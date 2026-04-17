#pragma once

#include <memory>
#include <openvr_driver.h>
#include <tobii/tobii_wearable.h>

class StarPatcher {
public:
	StarPatcher();
	void Initialize(uintptr_t mod);
	void Destroy();
private:
	static vr::EVRInitError ActivatePatch(uintptr_t thisptr, uint32_t unObjectId);
	static void WearableCallbackPatch(tobii_wearable_data_t* data, void* user_data);

	static uintptr_t m_moduleBase;
	static vr::VRInputComponentHandle_t m_eyeTrackingInput;
};