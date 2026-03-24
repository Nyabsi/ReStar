#pragma once

#include <memory>

#include <interfaces/IVRDisplayComponent.hpp>
#include <interfaces/IVRDriverDirectModeComponent.hpp>

class StarPatcher {
public:
	StarPatcher();
	void Initialize(uintptr_t mod);
	void Destroy();
private:
	static vr::EVRInitError ActivatePatch(void* thisptr, uint32_t unObjectId);
	static void* GetComponentPatch(void* thisptr, const char* name);

	uintptr_t m_moduleBase;
	static std::unique_ptr<IVRDisplayComponentWrapper> m_displayComponentWrapper;
	static std::unique_ptr<IVRDriverDirectModeComponentWrapper> m_directModeComponentWrapper;
};