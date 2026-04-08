#pragma once

#include <memory>
#include <openvr_driver.h>

class StarPatcher {
public:
	StarPatcher();
	void Initialize(uintptr_t mod);
	void Destroy();
private:
	static vr::EVRInitError ActivatePatch(uintptr_t thisptr, uint32_t unObjectId);

	static uintptr_t m_moduleBase;
};