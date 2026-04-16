#include "TrackedDeviceProvider.hpp"

#include <openvr_driver.h>
#include <windows.h>
#include <shlwapi.h>
#include "vrmath.hpp"

#include <sstream>
#include <fstream>

#include <chrono>
#include <iostream>
#include <functional>

#include <ITE_FW.h>
#include <starvr_user_api.h>

using namespace std::chrono_literals;

#pragma comment(lib, "Shlwapi.lib")

HMODULE lib = nullptr;
HmdDriverFactoryFn factory = nullptr;

static vr::DriverPose_t GetPose(uint32_t variant, uint32_t refreshRate)
{
	static vr::TrackedDeviceIndex_t trackerIndex = vr::k_unTrackedDeviceIndexInvalid;
	static bool trackerResolved = false;

	if (!trackerResolved)
	{
		auto props = vr::VRProperties();

		for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
		{
			vr::PropertyContainerHandle_t h =
				props->TrackedDeviceToPropertyContainer(i);

			if (h == vr::k_ulInvalidPropertyContainer)
				continue;

			vr::ETrackedPropertyError err;

			int32_t deviceClass =
				props->GetInt32Property(
					h,
					vr::Prop_DeviceClass_Int32,
					&err
				);

			if (deviceClass == vr::TrackedDeviceClass_GenericTracker && err == vr::TrackedProp_Success)
			{
				trackerIndex = i;
				trackerResolved = true;
				break;
			}
		}
	}

	vr::DriverPose_t pose{};

	pose.qWorldFromDriverRotation.w = 1.0f;
	pose.qDriverFromHeadRotation.w = 1.0f;
	pose.poseTimeOffset = 0.0;

	pose.shouldApplyHeadModel = false;
	pose.willDriftInYaw = false;
	pose.poseIsValid = false;
	pose.deviceIsConnected = false;

	pose.result = vr::TrackingResult_Running_OutOfRange;

	vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
	vr::VRServerDriverHost()->GetRawTrackedDevicePoses(1.0 / static_cast<float>(refreshRate), poses, vr::k_unMaxTrackedDeviceCount);

	if (trackerIndex != vr::k_unTrackedDeviceIndexInvalid)
	{
		const auto& tp = poses[trackerIndex];
		if (tp.bPoseIsValid)
		{
			vr::HmdQuaternion_t trackerQuat = HmdQuaternion_FromMatrix(tp.mDeviceToAbsoluteTracking);

			// XT variant assumes 90-degree rotation for the Vive Tracker
			if (variant == 1) {
				constexpr float deg = 90.0f * (float)M_PI / 180.0f;
				constexpr float half = deg * 0.5f;
				vr::HmdQuaternion_t qMountInv{ cosf(half), -sinf(half), 0.0f, 0.0f };

				pose.qRotation = Normalize(Multiply(trackerQuat, qMountInv));
			}
			else {
				pose.qRotation = trackerQuat;
			}

			double trackerPos[3] = {
				tp.mDeviceToAbsoluteTracking.m[0][3],
				tp.mDeviceToAbsoluteTracking.m[1][3],
				tp.mDeviceToAbsoluteTracking.m[2][3]
			};

			vr::HmdVector3_t localEyeOffset = { 0.0f, 0.0f, 0.0f };
			if (variant == 1) {
				localEyeOffset = { 0.0f, 78.999499999999998f, 52.927599999999998f }; // from EEPROM
			}

			vr::HmdVector3_t worldOffset = RotateVector(pose.qRotation, localEyeOffset);

			pose.vecPosition[0] = trackerPos[0] + (worldOffset.v[0] * 0.001f);
			pose.vecPosition[1] = trackerPos[1] + -(worldOffset.v[1] * 0.001f);
			pose.vecPosition[2] = trackerPos[2] + (worldOffset.v[2] * 0.001f);

			pose.vecVelocity[0] = tp.vVelocity.v[0];
			pose.vecVelocity[1] = tp.vVelocity.v[1];
			pose.vecVelocity[2] = tp.vVelocity.v[2];

			pose.poseIsValid = true;
			pose.deviceIsConnected = true;
			pose.result = vr::TrackingResult_Running_OK;
		}
	}

	return pose;
}

TrackedDeviceProvider::TrackedDeviceProvider()
{
	m_provider = nullptr;
	m_starPatcher = std::make_unique<StarPatcher>();
	m_headsetHandle = nullptr;
	m_currentBrightness = {};
	m_ipd = {};
	m_refreshRate = {};
	m_trackingVariant = {};
}

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    if (!pDriverContext)
    {
        return vr::VRInitError_Driver_Failed;
    }

	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

	wchar_t starVRPath[MAX_PATH] = L"C:\\Program Files (x86)\\StarVR\\OpenVR\\bin\\win64";
	wchar_t modulePath[MAX_PATH] = { };

	AddDllDirectory(starVRPath);
	PathCombineW(modulePath, starVRPath, L"driver_starvr.dll");

	lib = LoadLibraryExW(modulePath, nullptr, 
		LOAD_LIBRARY_SEARCH_DEFAULT_DIRS |
		LOAD_LIBRARY_SEARCH_USER_DIRS |
		LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
	);

    if (!lib)
    {
        DWORD err = GetLastError();

        wchar_t msg[512];
        wsprintfW(msg, L"LoadLibrary FAILED\nError code: %lu", err);

        MessageBoxW(nullptr, msg, L"Error", MB_OK);
        return vr::VRInitError_Driver_Failed;
    }

    factory = (HmdDriverFactoryFn)GetProcAddress(lib, "HmdDriverFactory");

    if (!factory)
    {
        MessageBoxW(nullptr, L"HmdDriverFactory not found", L"Error", MB_OK);
        return vr::VRInitError_Driver_Failed;
    }
	
    m_provider = static_cast<IServerTrackedDeviceProvider*>(
        factory(vr::IServerTrackedDeviceProvider_Version, nullptr)
    );

    if (!m_provider)
    {
        return vr::VRInitError_Driver_Failed;
    }

	m_starPatcher->Initialize(reinterpret_cast<uintptr_t>(lib));

    auto result = m_provider->Init(pDriverContext);

	ITE_ConnectToHMD(&m_headsetHandle, 0, 0);

	uint16_t brightness = {};
	ITE_BrightnessRead(m_headsetHandle, &brightness);
	m_currentBrightness = static_cast<float>((brightness >> 8) & 0xFF) / 255.0f;

	DisplayRefreshRate refreshRate = {};
	ITE_FPSSettingRead(m_headsetHandle, &refreshRate);

	vr::EVRSettingsError err = {};

	m_refreshRate = vr::VRSettings()->GetInt32(
		vr::k_pch_SteamVR_Section,
		"preferredRefreshRate",
		&err
	);

	if (err == vr::VRSettingsError_UnsetSettingHasNoDefault)
		m_refreshRate = refreshRate;
	
	m_ipd = vr::VRSettings()->GetFloat(
		vr::k_pch_SteamVR_Section,
		"ipd",
		&err
	);

	if (err == vr::VRSettingsError_UnsetSettingHasNoDefault) {
		float ipd;
		starvr::StarVR_User_GetIPD(&ipd);
		m_ipd = ipd / 1000.0f;
	}

	m_trackingVariant = vr::VRSettings()->GetInt32(
		"driver_restar",
		"hmdTrackerVariant",
		&err
	);

	if (err == vr::VRSettingsError_UnsetSettingHasNoDefault)
		m_trackingVariant = 1;

	auto temporaryRefreshRate = vr::VRSettings()->GetInt32(
		"driver_restar",
		"temporaryRefreshRate",
		&err
	);

	if (temporaryRefreshRate != -1 && err == vr::VRSettingsError_None) {
		switch (temporaryRefreshRate) {
			case 72:
				ITE_FPSSettingWrite(m_headsetHandle, REFRESH_RATE_72_HZ);
				break;
			case 75:
				ITE_FPSSettingWrite(m_headsetHandle, REFRESH_RATE_75_HZ);
				break;
			case 89:
				ITE_FPSSettingWrite(m_headsetHandle, REFRESH_RATE_89_HZ);
				break;
			case 90:
				ITE_FPSSettingWrite(m_headsetHandle, REFRESH_RATE_90_HZ);
				break;
			default:
				break;
		}

		vr::VRSettings()->SetInt32(
			"driver_restar",
			"temporaryRefreshRate",
			-1
		);

		return vr::VRInitError_Init_Retry; // restart to ensure proper refresh rate is applied
	}

	vr::VRSettings()->SetFloat(
		vr::k_pch_SteamVR_Section,
		"analogGain",
		powf(m_currentBrightness, 2.2f)
	);

	vr::VRSettings()->SetFloat(
		vr::k_pch_SteamVR_Section,
		"ipd",
		m_ipd
	);

	m_is_active_.exchange(true);
	m_update_thread_ = std::thread(&TrackedDeviceProvider::Update, this);
	 
    return result;
}

void TrackedDeviceProvider::Cleanup()
{
	m_starPatcher->Destroy();

	m_is_active_.exchange(false);
	if (m_update_thread_.joinable()) {
		m_update_thread_.join();
	}

	m_provider->Cleanup();

	ITE_DisconnectHMD(m_headsetHandle);

	VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* TrackedDeviceProvider::GetInterfaceVersions()
{
	static const char* const versions[] =
	{
		"IVRDisplayComponent_002",
		"IVRDriverDirectModeComponent_006",
		"IVRSettings_003",
		"ITrackedDeviceServerDriver_005",
		"IVRCameraComponent_003",
		"IServerTrackedDeviceProvider_004",
		"IVRWatchdogProvider_001",
		"IVRVirtualDisplay_002",
		"IVRDriverManager_001",
		"IVRResources_001",
		"IVRCompositorPluginProvider_001",
		"IVRIPCResourceManagerClient_002",

		nullptr
	};

	return versions;
}

void TrackedDeviceProvider::RunFrame()

{
	vr::VREvent_t event = {};
	while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(vr::VREvent_t))) 
	{
		switch (event.eventType) {
			case vr::EVREventType::VREvent_SteamVRSectionSettingChanged:
			{
				float newBrightness = powf(
					vr::VRSettings()->GetFloat(
						vr::k_pch_SteamVR_Section, 
						"analogGain"
					),
					1.0f / 2.2f
				);

				int32_t refresh = vr::VRSettings()->GetInt32(
					vr::k_pch_SteamVR_Section,
					"preferredRefreshRate"
				);

				float ipd = vr::VRSettings()->GetFloat(
					vr::k_pch_SteamVR_Section,
					"ipd"
				);

				if (newBrightness != m_currentBrightness)
				{
					m_currentBrightness = newBrightness;

					uint8_t brightness = static_cast<uint8_t>(m_currentBrightness * 255.0f);
					ITE_BrightnessWrite(m_headsetHandle, PANEL_LEFT, brightness);
					ITE_BrightnessWrite(m_headsetHandle, PANEL_RIGHT, brightness);
				}

				if (m_refreshRate != refresh)
				{
					m_refreshRate = refresh;

					vr::VRSettings()->SetInt32(
						"driver_restar",
						"temporaryRefreshRate",
						m_refreshRate
					);
				}

				if (m_ipd != ipd) {
					m_ipd = ipd;
					starvr::StarVR_User_SetIPD(ipd / 1000.0f);
				}

				break;
			}
		}
	}

	m_provider->RunFrame();
}

bool TrackedDeviceProvider::ShouldBlockStandbyMode()
{
	return true;
}

void TrackedDeviceProvider::EnterStandby()
{

}

void TrackedDeviceProvider::LeaveStandby()
{

}

void TrackedDeviceProvider::Update()
{
	while (m_is_active_) {
		vr::VRServerDriverHost()->TrackedDevicePoseUpdated(0, GetPose(m_trackingVariant, m_refreshRate), sizeof(vr::DriverPose_t));
		std::this_thread::sleep_for(1ms);
	}
}
