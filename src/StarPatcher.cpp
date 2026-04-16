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
    *((uint32_t*)(thisptr + 24)) = unObjectId;

    void** classPtr = reinterpret_cast<void**>(m_moduleBase + 0x3BE08); // this class manages displays / direct mode
    if (!*classPtr) {
        *classPtr = malloc(0x1038);
        memset(*classPtr, 0x0, 0x1038);
        *classPtr = ((void* (*)(void*))reinterpret_cast<void*>(m_moduleBase + 0x0DF30))(*classPtr);
    }

    *(void**)(thisptr + 640) = *classPtr;

    uint8_t unk1 = *((uint8_t*)(thisptr + 488));
    float unk2 = 0;
    if (unk1)
    {
        unk2 = *((float*)(thisptr + 492));
    }
    else
    {
        unk2 = -1.0f;
    }

    void* unk3 = ((void**)(thisptr + 104));
    uint8_t flag1 = *((uint8_t*)(thisptr + 236));
    uint8_t flag2 = *((uint8_t*)(thisptr + 237));
    float unk5 = *((float*)(thisptr + 168));
    float unk6 = 1.0f / *((float*)(thisptr + 172));

    if (((bool(*)(void*, void*, uint8_t, uint8_t, uint8_t, float, float, float))reinterpret_cast<void*>(m_moduleBase + 0x0EA80))(*(void**)(thisptr + 640), unk3, flag2, flag1, unk1, unk2, unk5, unk6))
    {
        ((bool(*)(uintptr_t, uint8_t, float))reinterpret_cast<void*>(m_moduleBase + 0x1A390))(thisptr, 0, -1.0f);

        float unk7 = *((float*)(thisptr + 232));
        float unk8 = *((float*)(thisptr + 228));
        float unk9 = *((float*)(thisptr + 216));
        uint32_t unk10 = *((uint32_t*)(thisptr + 208));
        uint32_t unk11 = *((uint32_t*)(thisptr + 212));

        if (((bool(*)(void*, float, float, float, uint32_t, uint32_t))reinterpret_cast<void*>(m_moduleBase + 0x10300))(*(void**)(thisptr + 640), unk7, unk8, unk9, unk10, unk11))
        {
            vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

            vr::VRProperties()->SetFloatProperty(container, vr::Prop_UserIpdMeters_Float, *((float*)(thisptr + 176)));
            vr::VRProperties()->SetFloatProperty(container, vr::Prop_UserHeadToEyeDepthMeters_Float, 0.0f);
            vr::VRProperties()->SetFloatProperty(container, vr::Prop_DisplayFrequency_Float, *((float*)(thisptr + 172)));
            vr::VRProperties()->SetFloatProperty(container, vr::Prop_SecondsFromVsyncToPhotons_Float, *((float*)(thisptr + 168)));
            vr::VRProperties()->SetBoolProperty(container, vr::Prop_DriverDirectModeSendsVsyncEvents_Bool, true);

            vr::VRProperties()->SetInt32Property(container, vr::Prop_ControllerRoleHint_Int32, 3);

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

            //vr::VRProperties()->SetInt32Property(container, vr::Prop_EdidVendorID_Int32, 0x7204);
            //vr::VRProperties()->SetInt32Property(container, vr::Prop_EdidProductID_Int32, 0x7530);

            vr::VRProperties()->SetBoolProperty(container, vr::Prop_DisplaySupportsAnalogGain_Bool, true);
            vr::VRProperties()->SetFloatProperty(container, vr::Prop_DisplayMinAnalogGain_Float, 0.0f);
            vr::VRProperties()->SetFloatProperty(container, vr::Prop_DisplayMaxAnalogGain_Float, 1.0f);

            uint8_t unk12 = *((uint8_t*)(thisptr + 496));
            if (unk12)
            {
                struct GCImage {
                    void* data;
                    uint32_t width;
                    uint32_t height;
                    uint32_t channels;
                }; // size 20

                GCImage* image = new GCImage;

                bool failed_load = false;

                std::string path = {};
                path += *(std::string*)(thisptr + 104);
                path += "\\resources\\gc\\StarVR_v0.gc";

                vr::VRDriverLog()->Log(path.c_str());

                if (((unsigned int (*)(void**, uint32_t*, uint32_t*, const char*, uint32_t))reinterpret_cast<void*>(m_moduleBase + 0x0B520))(&image->data, &image->width, &image->height, path.c_str(), 2))
                {
                    image->channels = 0;
                    failed_load = true;
                }
                else
                {
                    image->channels = 3;
                }

                if (failed_load)
                {
                    vr::VRDriverLog()->Log("Could not load GC image\n");
                }
                else
                {
                    float unk13 = *((float*)(thisptr + 500));
                    float unk14 = *((float*)(thisptr + 504));
                    float unk14a = *((float*)(thisptr + 508));
                    float unk14b = *((float*)(thisptr + 512));
                    if (((unsigned int(*)(void*, GCImage*, float, float, float, float))reinterpret_cast<void*>(m_moduleBase + 0x11EE0))(*(void**)(thisptr + 640), image, unk13, unk14, unk14a, unk14b))
                        vr::VRDriverLog()->Log("Compositor: Successfully initialized Ghost Correction\n");
                    else
                        vr::VRDriverLog()->Log("Compositor: Could not initialize Ghost Correction\n");
                }
                if (image)
                    free(image);
            }

            // this sets TrackedDeviceDisplayTransformUpdated, which btw doesn't exist in v006.
            ((void(*)(uintptr_t))reinterpret_cast<void*>(m_moduleBase + 0x1A980))(thisptr);
            ((void(*)(uintptr_t))reinterpret_cast<void*>(m_moduleBase + 0x1AE60))(thisptr);

            vr::VRDriverLog()->Log("Loading Mura correction files...\n");

            std::string a2a = "C:\\Users\\Administrator\\AppData\\Local";

            uint8_t upperflag = *((uint8_t*)(thisptr + 237));
            uint8_t lowerflag = *((uint8_t*)(thisptr + 236));

            uint32_t heightprobably = *((uint32_t*)(thisptr + 200));
            uint32_t widthprobably = *((uint32_t*)(thisptr + 204));

            std::string serialnumber1 = *(std::string*)(thisptr + 240);
            std::string serialnumber2 = *(std::string*)(thisptr + 272);

            uint8_t v128[0x50] = {};

            if (((int(*)(void*, uint32_t, uint32_t, uint32_t, std::string, std::string, uint8_t, uint8_t, std::string))reinterpret_cast<void*>(m_moduleBase + 0x225E0))(v128, upperflag ? 1195 : 1792, heightprobably / 2, widthprobably, serialnumber1, serialnumber2, lowerflag, upperflag, a2a))
            {
                vr::VRDriverLog()->Log("Successfully loaded Mura corrections from disk and created lookup table.\n");
                // TODO: validate, total guess work for now.
                if ( ((int(*)(uintptr_t, void*, void*, void*))reinterpret_cast<void*>(m_moduleBase + 0x11C30))(*(uintptr_t*)(thisptr + 640), v128, v128 + 0x18, v128 + 0x30) )
                    vr::VRDriverLog()->Log("Compositor: Successfully initialized Mura Correction\n");
                else
                    vr::VRDriverLog()->Log("Compositor: Could not initialize Mura Correction\n");
            }
            else
            {
                vr::VRDriverLog()->Log("Failed to load Mura corrections from disk and generating lookup table.\n");
            }

            // here we're creating thread that runs periodic nvidia timer
            uintptr_t v53 = *(uintptr_t*)(thisptr + 640);
            if (v53 && *(uint8_t*)(v53 + 120) && !*(uint8_t*)(v53 + 288))
            {
                *(uint32_t*)(v53 + 360) = unObjectId;
                *(uint8_t*)(v53 + 288) = 1;

                uintptr_t* v54 = (uintptr_t*)malloc(8);
                *v54 = v53;

                std::mutex mtx;
                std::condition_variable cv;
                bool ready = false;

                std::thread thr([v54, &mtx, &cv, &ready]() {
                    uintptr_t obj = *v54;

                    mtx.lock();
                    ready = true;
                    mtx.unlock();
                    cv.notify_one();

                    ((void(*)(uintptr_t))reinterpret_cast<void*>(m_moduleBase + 0x125E0))(obj);

                    free(v54);
                });

                std::unique_lock<std::mutex> lk(mtx);
                cv.wait(lk, [&ready]() { return ready; });

                if (*(uint32_t*)(v53 + 304))
                    std::terminate();

                *(std::thread*)(v53 + 296) = std::move(thr);
            }

            void* eyeTrackClass = ((void* (*)(uint32_t))reinterpret_cast<void*>(m_moduleBase + 0x20DD0))(unObjectId);
            *(void**)(thisptr + 656) = eyeTrackClass;

            if (((int(*)(void*, std::string))reinterpret_cast<void*>(m_moduleBase + 0x20E90))(eyeTrackClass, *(std::string*)(thisptr + 104)))
                vr::VRDriverLog()->Log("Eye Tracking init failed : %d\n");
            *((uint32_t*)(thisptr + 848)) = 0;

            return vr::VRInitError_None; // return OK
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