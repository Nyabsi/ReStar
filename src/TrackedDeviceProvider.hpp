#pragma once

#include <memory>
#include <thread>

#include <openvr_driver.h>

#include <StarPatcher.hpp>

using HmdDriverFactoryFn = void* (*)(const char*, int*);

class TrackedDeviceProvider : public vr::IServerTrackedDeviceProvider
{
public:
	TrackedDeviceProvider();

	vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext) override;

	void Cleanup() override;

	const char* const* GetInterfaceVersions() override;

	void RunFrame() override;

	bool ShouldBlockStandbyMode() override;

	void EnterStandby() override;

	void LeaveStandby() override;
private:
	IServerTrackedDeviceProvider* m_provider;
	std::unique_ptr<StarPatcher> m_starPatcher;
	void* m_headsetHandle;

	void Update();
	std::atomic<bool> m_is_active_;
	std::thread m_update_thread_;

	float m_currentBrightness;
	int32_t m_refreshRate;
	float m_ipd;
	int32_t m_trackingVariant;
};