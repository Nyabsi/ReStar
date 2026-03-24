#include <memory>
#include <string.h>

#include <openvr_driver.h>
#include <TrackedDeviceProvider.hpp>

#ifdef _WIN32
#define EXPORT_FUNC extern "C" __declspec(dllexport)
#else
#define EXPORT_FUNC extern "C"
#endif

std::unique_ptr<TrackedDeviceProvider> g_trackedDeviceProvider = std::make_unique<TrackedDeviceProvider>();

EXPORT_FUNC void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode)
{
    if (strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName) == 0) {
        return static_cast<vr::IServerTrackedDeviceProvider*>(g_trackedDeviceProvider.get());
    }

    if (pReturnCode)
        *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    return nullptr;
}