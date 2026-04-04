#pragma once

#include <openvr_driver.h>

#pragma warning(push)
#pragma warning(disable : 4100)

namespace vrstub
{
	struct SwapTextureSetDesc_t
	{
		uint32_t nWidth;
		uint32_t nHeight;
		uint32_t nFormat;
		uint32_t nSampleCount;
	};

	struct SwapTextureSet_t
	{
		vr::SharedTextureHandle_t rSharedTextureHandles[2];
		uint32_t unTextureFlags;
	};

	class IVRDriverDirectModeComponent_009
	{
	public:
		struct SubmitLayerPerEye_t
		{
			vr::SharedTextureHandle_t hTexture, hDepthTexture;
			vr::VRTextureBounds_t bounds;
			vr::HmdMatrix44_t mProjection;
			vr::HmdMatrix34_t mHmdPose;
			float flHmdPosePredictionTimeInSecondsFromNow;
		};

		struct Throttling_t
		{
			uint32_t nFramesToThrottle;
			uint32_t nAdditionalFramesToPredict;
		};

		virtual void CreateSwapTextureSet(uint32_t unPid, const SwapTextureSetDesc_t* pSwapTextureSetDesc, SwapTextureSet_t* pOutSwapTextureSet) {}
		virtual void DestroySwapTextureSet(vr::SharedTextureHandle_t sharedTextureHandle) {}
		virtual void DestroyAllSwapTextureSets(uint32_t unPid) {}
		virtual void GetNextSwapTextureSetIndex(vr::SharedTextureHandle_t sharedTextureHandles[2], uint32_t(*pIndices)[2]) {}
		virtual void SubmitLayer(const SubmitLayerPerEye_t(&perEye)[2]) {}
		virtual void Present(vr::SharedTextureHandle_t syncTexture) {}
		virtual void PostPresent(const Throttling_t* pThrottling) {}
		virtual void GetFrameTiming(vr::DriverDirectMode_FrameTiming* pFrameTiming)
		{
			pFrameTiming->m_nReprojectionFlags = 0;
		}
	};

	class IVRDriverDirectModeComponent_005
	{
	public:
		struct SubmitLayerPerEye_t
		{
			vr::SharedTextureHandle_t hTexture, hDepthTexture;
			vr::VRTextureBounds_t bounds;
			vr::HmdMatrix44_t mProjection;
		};

		virtual void CreateSwapTextureSet(uint32_t unPid, const SwapTextureSetDesc_t* pSwapTextureSetDesc, vr::SharedTextureHandle_t(*pSharedTextureHandles)[3]) {}
		virtual void DestroySwapTextureSet(vr::SharedTextureHandle_t sharedTextureHandle) {}
		virtual void DestroyAllSwapTextureSets(uint32_t unPid) {}
		virtual void GetNextSwapTextureSetIndex(vr::SharedTextureHandle_t sharedTextureHandles[2], uint32_t(*pIndices)[2]) {}
		virtual void SubmitLayer(const SubmitLayerPerEye_t(&perEye)[2], const vr::HmdMatrix34_t* pPose) {}
		virtual void Present(vr::SharedTextureHandle_t syncTexture) {}
		virtual void PostPresent() {}
		virtual void GetFrameTiming(vr::DriverDirectMode_FrameTiming* pFrameTiming) {}
	};

}

#pragma warning(pop)

class IVRDriverDirectModeComponentWrapper : public vrstub::IVRDriverDirectModeComponent_009
{
	vrstub::IVRDriverDirectModeComponent_005* m_original;
	float m_ipd;
	bool m_autoIpdEnabled;

public:
	IVRDriverDirectModeComponentWrapper(vrstub::IVRDriverDirectModeComponent_005* original)
		: m_original(original)
	{
		m_autoIpdEnabled = vr::VRSettings()->GetBool(
			"driver_StarVR",
			"autoIpdEnabled"
		);
	}

	void CreateSwapTextureSet(uint32_t unPid, const vrstub::SwapTextureSetDesc_t* pDesc,
		vrstub::SwapTextureSet_t* pOutSwapTextureSet) override
	{
		vr::SharedTextureHandle_t handles[3] = {};
		m_original->CreateSwapTextureSet(unPid, pDesc, &handles);

		pOutSwapTextureSet->rSharedTextureHandles[0] = handles[0];
		pOutSwapTextureSet->rSharedTextureHandles[1] = handles[1];
		pOutSwapTextureSet->rSharedTextureHandles[2] = handles[2];
	}

	void DestroySwapTextureSet(vr::SharedTextureHandle_t h) override {
		m_original->DestroySwapTextureSet(h);
	}

	void DestroyAllSwapTextureSets(uint32_t unPid) override {
		m_original->DestroyAllSwapTextureSets(unPid);
	}

	void GetNextSwapTextureSetIndex(vr::SharedTextureHandle_t h[2],
		uint32_t(*pIndices)[2]) override {
		m_original->GetNextSwapTextureSetIndex(h, pIndices);
	}

	void SubmitLayer(const SubmitLayerPerEye_t(&perEye)[2]) override
	{
		vrstub::IVRDriverDirectModeComponent_005::SubmitLayerPerEye_t v5[2] = {};

		for (int i = 0; i < 2; i++) {
			v5[i].hTexture = perEye[i].hTexture;
			v5[i].hDepthTexture = perEye[i].hDepthTexture;
			v5[i].bounds = perEye[i].bounds;
			v5[i].mProjection = perEye[i].mProjection;
		}

		m_original->SubmitLayer(v5, &perEye[1].mHmdPose);
	}

	void Present(vr::SharedTextureHandle_t syncTexture) override {
		m_original->Present(syncTexture);
	}

	void PostPresent(const Throttling_t*) override {
		m_original->PostPresent();
	}

	void GetFrameTiming(vr::DriverDirectMode_FrameTiming* pFrameTiming) override {
		m_original->GetFrameTiming(pFrameTiming);
		pFrameTiming->m_nReprojectionFlags = 0;
	}
};