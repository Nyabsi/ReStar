#pragma once

#include <openvr_driver.h>

#pragma warning(push)
#pragma warning(disable : 4100)

namespace vrstub
{
	class IVRDisplayComponent_002
	{
	public:

		virtual void GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) = 0;
		virtual bool IsDisplayOnDesktop() = 0;
		virtual bool IsDisplayRealDisplay() = 0;
		virtual void GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight) = 0;
		virtual void GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) = 0;
		virtual void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) = 0;
		virtual vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) = 0;
	};

	class IVRDisplayComponent_003
	{
	public:

		virtual void GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) = 0;
		virtual bool IsDisplayOnDesktop() = 0;
		virtual bool IsDisplayRealDisplay() = 0;
		virtual void GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight) = 0;
		virtual void GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) = 0;
		virtual void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) = 0;
		virtual vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) = 0;
		virtual bool ComputeInverseDistortion(vr::HmdVector2_t* pResult, vr::EVREye eEye, uint32_t unChannel, float fU, float fV) = 0;
	};
}

#pragma warning(pop)

class IVRDisplayComponentWrapper : public vrstub::IVRDisplayComponent_003
{
	vrstub::IVRDisplayComponent_002* m_pOriginal;

public:
	IVRDisplayComponentWrapper(vrstub::IVRDisplayComponent_002* original)
		: m_pOriginal(original) {
	}

	void GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override {
		*pnX = 0;
		*pnY = 0;
		*pnWidth = 1792;
		*pnHeight = 2240;
	}
	bool IsDisplayOnDesktop() override {
		return false;
	}
	bool IsDisplayRealDisplay() override {
		return true;
	}
	void GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight) override {
		*pnWidth = 1792;
		*pnHeight = 2240 * 2;
	}
	void GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override {
		m_pOriginal->GetEyeOutputViewport(eEye, pnX, pnY, pnWidth, pnHeight);
	}
	void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) override {
		m_pOriginal->GetProjectionRaw(eEye, pfLeft, pfRight, pfTop, pfBottom);
	}
	vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) override {
		return m_pOriginal->ComputeDistortion(eEye, fU, fV);
	}

	bool ComputeInverseDistortion(vr::HmdVector2_t* pResult, vr::EVREye eEye, uint32_t unChannel, float fU, float fV) override {
		(void)pResult;
		(void)eEye;
		(void)unChannel;
		(void)fU;
		(void)fV;

		return false;
	}
};