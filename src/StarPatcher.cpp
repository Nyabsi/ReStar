#include "StarPatcher.hpp"

#include <functional>
#include <openvr_driver.h>

#include <ShlObj.h>
#include <MinHook.h>
#include <mutex>
#include <format>

using OpenVR_Activate_tmpl = vr::EVRInitError(__fastcall*)(uintptr_t, uint32_t);
OpenVR_Activate_tmpl OpenVR_Activate = nullptr;

uintptr_t StarPatcher::m_moduleBase = {};

StarPatcher::StarPatcher()
{

}

#define SVR_ACTIVATE_2_99_9999 0x183C0

void StarPatcher::Initialize(uintptr_t mod)
{
	m_moduleBase = mod;

	MH_STATUS status;

	status = MH_Initialize();

	status = MH_CreateHook(
		reinterpret_cast<void*>(m_moduleBase + SVR_ACTIVATE_2_99_9999),
		reinterpret_cast<void*>(StarPatcher::ActivatePatch),
		reinterpret_cast<void**>(&OpenVR_Activate)
	);

	status = MH_EnableHook(MH_ALL_HOOKS);
}

void StarPatcher::Destroy()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}

vr::EVRInitError __fastcall StarPatcher::ActivatePatch(uintptr_t thisptr, uint32_t unObjectId)
{
    *((uint32_t*)(thisptr + 24)) = unObjectId; // set objectId to unObjectId in the original class

    // Since the root path has changed, force it back to StarVR so util programs load correctly
    *((std::string*)(thisptr + 104)) = "C:\\Program Files (x86)\\StarVR\\OpenVR";
    // Overwrite the device IPD so we can load the proper mesh when automatic IPD is turned off
    *((float*)(thisptr + 176)) = vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, "ipd") / 1000.0f;

    void** displayManagerPtr = reinterpret_cast<void**>(m_moduleBase + 0x3BE08);
    if (!*displayManagerPtr) {
        *displayManagerPtr = malloc(0x1038);
        memset(*displayManagerPtr, 0x0, 0x1038);
        *displayManagerPtr = ((void* (*)(void*))reinterpret_cast<void*>(m_moduleBase + 0x0DF30))(*displayManagerPtr);
    }

    *(void**)(thisptr + 640) = *displayManagerPtr;

    uint8_t legacyMode = *((uint8_t*)(thisptr + 488));

    float fovLimit = 0;
    if (legacyMode)
    {
        fovLimit = *((float*)(thisptr + 492)); // set to 140
    }
    else
    {
        fovLimit = -1.0f;
    }

    // these flags configure the horizontal and vertical resolution of the device in various of places. it's hardcoded to 0101
    uint8_t horizontalFlag = *((uint8_t*)(thisptr + 236));
    uint8_t verticalFlag = *((uint8_t*)(thisptr + 237));

    void* unkPtr = ((void**)(thisptr + 104)); // haven't figured out yet, EEPROM related.
    float secondsFromVsyncToPhotons = *((float*)(thisptr + 168)); // read from EEPROM
    float displayFrequency = 1.0f / *((float*)(thisptr + 172)); // read from EEPROM

    // Initialize NVIDIA Direct Mode via NVAPI
    if (((bool(*)(void*, void*, uint8_t, uint8_t, uint8_t, float, float, float))reinterpret_cast<void*>(m_moduleBase + 0x0EA80))(*(void**)(thisptr + 640), unkPtr, verticalFlag, horizontalFlag, legacyMode, fovLimit, secondsFromVsyncToPhotons, displayFrequency))
    {
        // Initialize Distortion Mesh + Set IPD (third argument)
        ((bool(*)(uintptr_t, uint8_t, float))reinterpret_cast<void*>(m_moduleBase + 0x1A390))(thisptr, 0, -1.0f);

        float horizontalFov = *((float*)(thisptr + 232)); // 138.68835
        float verticalFov = *((float*)(thisptr + 228)); // 126.77851
        float lensCantingAngle = *((float*)(thisptr + 216)); // -24.247019

        uint32_t renderWidth = *((uint32_t*)(thisptr + 208));
        uint32_t renderHeight = *((uint32_t*)(thisptr + 212));

        // Initialize the distortion shader
        if (((bool(*)(void*, float, float, float, uint32_t, uint32_t))reinterpret_cast<void*>(m_moduleBase + 0x10300))(*(void**)(thisptr + 640), horizontalFov, verticalFov, lensCantingAngle, renderWidth, renderHeight))
        {
            vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

            vr::VRProperties()->SetFloatProperty(container, vr::Prop_UserIpdMeters_Float, *((float*)(thisptr + 176)));
            vr::VRProperties()->SetFloatProperty(container, vr::Prop_UserHeadToEyeDepthMeters_Float, 0.0f);
            vr::VRProperties()->SetFloatProperty(container, vr::Prop_DisplayFrequency_Float, *((float*)(thisptr + 172)));
            vr::VRProperties()->SetFloatProperty(container, vr::Prop_SecondsFromVsyncToPhotons_Float, secondsFromVsyncToPhotons);
            vr::VRProperties()->SetBoolProperty(container, vr::Prop_DriverDirectModeSendsVsyncEvents_Bool, true);

            vr::VRProperties()->SetInt32Property(container, vr::Prop_ControllerRoleHint_Int32, 3);

            vr::VRProperties()->SetStringProperty(container, vr::Prop_TrackingSystemName_String, "starvr_one");
            vr::VRProperties()->SetStringProperty(container, vr::Prop_ModelNumber_String, "StarVR One");
            vr::VRProperties()->SetStringProperty(container, vr::Prop_SerialNumber_String, "starvr_one_headset"); // I've redacted Serial Number to avoid this to be used for tracking...
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

            // StarVR vendorId & productId, if we want to disable NVIDIA Direct Mode and let SteamVR take over :3
            //vr::VRProperties()->SetInt32Property(container, vr::Prop_EdidVendorID_Int32, 0x7204);
            //vr::VRProperties()->SetInt32Property(container, vr::Prop_EdidProductID_Int32, 0x7530);

            vr::VRProperties()->SetBoolProperty(container, vr::Prop_DisplaySupportsAnalogGain_Bool, true);
            vr::VRProperties()->SetFloatProperty(container, vr::Prop_DisplayMinAnalogGain_Float, 0.0f);
            vr::VRProperties()->SetFloatProperty(container, vr::Prop_DisplayMaxAnalogGain_Float, 1.0f);

            uint8_t settingsInitialized = *((uint8_t*)(thisptr + 496));
            if (settingsInitialized && !vr::VRSettings()->GetBool("driver_restar", "disableGc"))
            {
                struct GCImage {
                    void* data;
                    uint32_t width;
                    uint32_t height;
                    uint32_t channels;
                }; // 0x14

                GCImage image;

                bool failed_load = false;

                std::string path = {};
                path += *(std::string*)(thisptr + 104); // driver root
                path += "\\resources\\gc\\StarVR_v0.gc";

                // Load ghost correction image from disk
                if (((unsigned int (*)(void**, uint32_t*, uint32_t*, const char*, uint32_t))reinterpret_cast<void*>(m_moduleBase + 0x0B520))(&image.data, &image.width, &image.height, path.c_str(), 2))
                {
                    image.channels = 0;
                    failed_load = true;
                }
                else
                {
                    image.channels = 3;
                }

                if (failed_load)
                {
                    vr::VRDriverLog()->Log("Could not load GC image\n");
                }
                else
                {
                    float unk1 = *((float*)(thisptr + 500));  // 52
                    float unk2 = *((float*)(thisptr + 504));  // 6
                    float unk3 = *((float*)(thisptr + 508));  // 0.93
                    float unk4 = *((float*)(thisptr + 512));  // 0

                    // Initialize ghost correction shader
                    if (((unsigned int(*)(void*, GCImage, float, float, float, float))reinterpret_cast<void*>(m_moduleBase + 0x11EE0))(*(void**)(thisptr + 640), image, unk1, unk2, unk3, unk4))
                        vr::VRDriverLog()->Log("Compositor: Successfully initialized Ghost Correction\n");
                    else
                        vr::VRDriverLog()->Log("Compositor: Could not initialize Ghost Correction\n");
                }
            }

            // sets TrackedDeviceDisplayTransformUpdated
            ((void(*)(uintptr_t))reinterpret_cast<void*>(m_moduleBase + 0x1A980))(thisptr);
            // Initializes Hidden Area Mesh if Legacy Mode is in use
            ((void(*)(uintptr_t))reinterpret_cast<void*>(m_moduleBase + 0x1AE60))(thisptr);

            if (!vr::VRSettings()->GetBool("driver_restar", "disableMc"))
            {
                vr::VRDriverLog()->Log("Loading Mura correction files...\n");

                auto GetLocalAppData = []() -> std::string
                {
                    PWSTR path = nullptr;

                    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path)))
                    {
                        std::wstring wpath(path);
                        CoTaskMemFree(path);

                        return std::string(wpath.begin(), wpath.end());
                    }

                    return "";
                };

                std::string path = GetLocalAppData();

                uint32_t width = *((uint32_t*)(thisptr + 200));
                uint32_t height = *((uint32_t*)(thisptr + 204));

                std::string leftPanelSerial = *(std::string*)(thisptr + 240);
                std::string rightPanelSerial = *(std::string*)(thisptr + 272);

                struct MuraCorrection {
                    // I don't know exact channel format, but each channel contains information about formats, colours, resolution and bla bla.
                    uint8_t channel_r[0x24];
                    uint8_t channel_g[0x24];
                    uint8_t channel_b[0x24];
                    uint8_t unk36;
                    uint8_t unk40;
                    uint8_t unk44;
                    uint8_t unk48;
                    uint8_t reserved0[0x04];
                }; // 0x50

                MuraCorrection correction = {};
                // Load mura correction files
                if (((unsigned int(*)(void*, uint32_t, uint32_t, uint32_t, std::string, std::string, uint8_t, uint8_t, std::string))reinterpret_cast<void*>(m_moduleBase + 0x225E0))(&correction, verticalFlag ? 1195 : 1792, width / 2, height, leftPanelSerial, rightPanelSerial, horizontalFlag, verticalFlag, path))
                {
                    vr::VRDriverLog()->Log("Successfully loaded Mura corrections from disk and created lookup table.\n");
                    // Initialize mura correction
                    if (((unsigned int(*)(uintptr_t, void*, void*, void*))reinterpret_cast<void*>(m_moduleBase + 0x11C30))(*(uintptr_t*)(thisptr + 640), correction.channel_r, correction.channel_g, correction.channel_b))
                        vr::VRDriverLog()->Log("Compositor: Successfully initialized Mura Correction\n");
                    else
                        vr::VRDriverLog()->Log("Compositor: Could not initialize Mura Correction\n");
                }
                else
                {
                    vr::VRDriverLog()->Log("Failed to load Mura corrections from disk and generating lookup table.\n");
                }
            }

            // here we're creating thread that runs periodic nvidia timer
            uintptr_t displayPtr = *(uintptr_t*)(thisptr + 640);
            // check if the ptr is valid, direct mode is initialized (+120) and we haven't created thread yet (+288)
            if (displayPtr && *(uint8_t*)(displayPtr + 120) && !*(uint8_t*)(displayPtr + 288))
            {
                *(uint32_t*)(displayPtr + 360) = unObjectId;
                *(uint8_t*)(displayPtr + 288) = 1;

                uintptr_t* tmpPtrAlloc = (uintptr_t*)malloc(8);
                *tmpPtrAlloc = displayPtr;

                std::mutex mtx;
                std::condition_variable cv;
                bool ready = false;

                std::thread thr([tmpPtrAlloc, &mtx, &cv, &ready]() {
                    uintptr_t ptr = *tmpPtrAlloc;

                    mtx.lock();
                    ready = true;
                    mtx.unlock();
                    cv.notify_one();

                    // display frame to the screen
                    ((void(*)(uintptr_t))reinterpret_cast<void*>(m_moduleBase + 0x125E0))(ptr);
                    free(tmpPtrAlloc);
                });

                std::unique_lock<std::mutex> lk(mtx);
                cv.wait(lk, [&ready]() { return ready; });

                if (*(uint32_t*)(displayPtr + 304))
                    std::terminate();

                *(std::thread*)(displayPtr + 296) = std::move(thr);
            }

            void* eyeTrackerClassPtr = ((void* (*)(std::thread*))reinterpret_cast<void*>(m_moduleBase + 0x20DD0))((std::thread*)(displayPtr + 296));
            *(void**)(thisptr + 656) = eyeTrackerClassPtr;

            int result = {};
            // Initialize Tobii eye tracking
            result = ((int(*)(void*, std::string))reinterpret_cast<void*>(m_moduleBase + 0x20E90))(eyeTrackerClassPtr, *(std::string*)(thisptr + 104));
            if (result)
                vr::VRDriverLog()->Log(std::format("Eye Tracking init failed : %d\n", result).c_str());

            *((uint32_t*)(thisptr + 848)) = 0;

            return vr::VRInitError_None;
        }
        else
        {
            vr::VRDriverLog()->Log("Set recommanded Target size and creation of associated resources failed\n");
            return vr::VRInitError_Driver_Failed;
        }
    }
    else
    {
        vr::VRDriverLog()->Log("Direct Mode init failed\n");
        return vr::VRInitError_Driver_Failed;
    }
}