#include "StarPatcher.hpp"

#include <functional>
#include <openvr_driver.h>

#include <MinHook.h>

using OpenVR_Activate_tmpl = vr::EVRInitError(__fastcall*)(void*, uint32_t);
OpenVR_Activate_tmpl OpenVR_Activate = nullptr;

using OpenVR_GetComponent_tmpl = void* (__fastcall*)(void*, const char*);
OpenVR_GetComponent_tmpl OpenVR_GetComponent = nullptr;

std::unique_ptr<IVRDisplayComponentWrapper> StarPatcher::m_displayComponentWrapper = nullptr;
std::unique_ptr<IVRDriverDirectModeComponentWrapper> StarPatcher::m_directModeComponentWrapper = nullptr;

StarPatcher::StarPatcher()
{
	m_moduleBase = {};
}

void StarPatcher::Initialize(uintptr_t mod)
{
	m_moduleBase = mod;

	MH_STATUS status;

	status = MH_Initialize();

	status = MH_CreateHook(
		reinterpret_cast<void*>(m_moduleBase + 0x16E60),
		reinterpret_cast<void*>(StarPatcher::ActivatePatch),
		reinterpret_cast<void**>(&OpenVR_Activate)
	);

	status = MH_CreateHook(
		reinterpret_cast<void*>(m_moduleBase + 0x18560),
		reinterpret_cast<void*>(StarPatcher::GetComponentPatch),
		reinterpret_cast<void**>(&OpenVR_GetComponent)
	);

	status = MH_EnableHook(MH_ALL_HOOKS);
}

void StarPatcher::Destroy()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}

vr::EVRInitError __fastcall StarPatcher::ActivatePatch(void* thisptr, uint32_t unObjectId)
{
	vr::EVRInitError result = OpenVR_Activate(thisptr, unObjectId);
	vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

	vr::VRProperties()->SetStringProperty(container, vr::Prop_TrackingSystemName_String, "starvr_one");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_ModelNumber_String, "StarVR One");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_SerialNumber_String, "starvr_one_headset");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_ManufacturerName_String, "Starbreeze");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_RenderModelName_String, "generic_hmd");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_ControllerType_String, "starvr_one_hmd");

	vr::VRProperties()->SetStringProperty(container, vr::Prop_TrackingFirmwareVersion_String, "1.0");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_HardwareRevision_String, "1.0");

	vr::VRProperties()->SetInt32Property(container, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_HMD);
	vr::VRProperties()->SetInt32Property(container, vr::Prop_HmdTrackingStyle_Int32, vr::HmdTrackingStyle_Lighthouse);

	vr::VRProperties()->SetUint64Property(container, vr::Prop_CurrentUniverseId_Uint64, 64);
	vr::VRProperties()->SetStringProperty(container, vr::Prop_InputProfilePath_String, "{restar}/input/starvr_one_profile.json");

	vr::VRProperties()->SetBoolProperty(container, vr::Prop_NeverTracked_Bool, false);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_Identifiable_Bool, false);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_IsOnDesktop_Bool, false);

	vr::VRProperties()->SetFloatProperty(container, vr::Prop_DashboardScale_Float, 0.9f);

	vr::VRProperties()->SetBoolProperty(container, vr::Prop_CanUnifyCoordinateSystemWithHmd_Bool, true);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_ContainsProximitySensor_Bool, false);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_UseAdvancedPrediction_Bool, true);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_ForceSystemLayerUseAppPoses_Bool, true);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_Hmd_AllowsClientToControlTextureIndex, true);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_Hmd_AllowSupersampleFiltering_Bool, true);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_Hmd_SupportsAppThrottling_Bool, true);

	vr::VRProperties()->SetBoolProperty(container, vr::Prop_HasDisplayComponent_Bool, true);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_HasCameraComponent_Bool, false);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_HasDriverDirectModeComponent_Bool, true);
	vr::VRProperties()->SetBoolProperty(container, vr::Prop_HasVirtualDisplayComponent_Bool, false);

	vr::VRProperties()->SetStringProperty(container, vr::Prop_NamedIconPathDeviceOff_String, "{restar}/icons/headset_status_off.png");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_NamedIconPathDeviceSearching_String, "{restar}/icons/headset_status_searching.gif");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{restar}/icons/headset_status_searching_alert.gif");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_NamedIconPathDeviceReady_String, "{restar}/icons/headset_status_ready.png");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{restar}/icons/headset_status_ready_alert.png");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_NamedIconPathDeviceNotReady_String, "{restar}/icons/headset_status_error.png");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_NamedIconPathDeviceStandby_String, "{restar}/icons/headset_status_standby.png");
	vr::VRProperties()->SetStringProperty(container, vr::Prop_NamedIconPathDeviceStandbyAlert_String, "{restar}/icons/headset_status_standby_alert.png");

	std::vector<float> available_frametimes = { 72.0f, 75.0f, 89.0f, 90.0f };
	vr::VRProperties()->SetPropertyVector(container, vr::Prop_DisplayAvailableFrameRates_Float_Array, vr::k_unFloatPropertyTag, &available_frametimes);

	vr::VRProperties()->SetBoolProperty(container, vr::Prop_DisplaySupportsAnalogGain_Bool, true);
	vr::VRProperties()->SetFloatProperty(container, vr::Prop_DisplayMinAnalogGain_Float, 0.0f);
	vr::VRProperties()->SetFloatProperty(container, vr::Prop_DisplayMaxAnalogGain_Float, 1.0f);

	return result;
}

void* __fastcall StarPatcher::GetComponentPatch(void* thisptr, const char* name)
{
	if (strcmp(name, "IVRDisplayComponent_003") == 0) {
		if (m_displayComponentWrapper == nullptr)
			m_displayComponentWrapper = std::make_unique<IVRDisplayComponentWrapper>(
				reinterpret_cast<vrstub::IVRDisplayComponent_002*>(reinterpret_cast<uintptr_t>(thisptr) + 8)
			);
		return m_displayComponentWrapper.get();
	}
	if (strcmp(name, "IVRDriverDirectModeComponent_009") == 0) {
		if (m_directModeComponentWrapper == nullptr)
			m_directModeComponentWrapper = std::make_unique<IVRDriverDirectModeComponentWrapper>(
				reinterpret_cast<vrstub::IVRDriverDirectModeComponent_005*>(reinterpret_cast<uintptr_t>(thisptr) + 16)
			);
		return m_directModeComponentWrapper.get();
	}

	return OpenVR_GetComponent(thisptr, name);
}
