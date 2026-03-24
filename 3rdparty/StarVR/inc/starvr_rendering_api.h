#pragma once
#include "starvr_common_define.h"


#if defined(_WIN32)
#define STARVR_DLL_EXPORT extern "C" __declspec( dllexport )
#define STARVR_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define STARVR_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define STARVR_DLL_IMPORT extern "C" 
#else
#error "Unsupported Platform."
#endif
namespace starvr {
	STARVR_DLL_EXPORT StarVRError StarVR_Rendering_SetMultiView();

	STARVR_DLL_EXPORT StarVRError StarVR_Rendering_GetHiddenArea_MultiView(starvr::EVREye eEye, vr::HmdVector2_t *pVertexData, uint32_t *unTriangleCount, vr::EHiddenAreaMeshType type = vr::EHiddenAreaMeshType::k_eHiddenAreaMesh_Standard);
	STARVR_DLL_EXPORT StarVRError StarVR_Rendering_GetEyeToHeadTransform_MultiView(starvr::EVREye eEye, vr::HmdMatrix34_t* transform);
	STARVR_DLL_EXPORT StarVRError StarVR_Rendering_GetProjectionMatrix_MultiView(starvr::EVREye eEye, float fNearZ, float fFarZ, vr::HmdMatrix44_t* projection);
	STARVR_DLL_EXPORT StarVRError StarVR_Rendering_GetProjectionRaw_MultiView(starvr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom);
	STARVR_DLL_EXPORT StarVRError StarVR_Rendering_GetGazePoint(starvr::EVREye eEye, vr::HmdVector3_t gazeDirection, vr::HmdVector2_t *point);
	STARVR_DLL_EXPORT StarVRError StarVR_Rendering_GetGazeProjection(starvr::EVREye eEye, float fNearZ, float fFarZ, float foveatedFactor ,float backgroundFactor, vr::HmdVector3_t gazeDirection, vr::HmdMatrix44_t* projection);
	STARVR_DLL_EXPORT StarVRError StarVR_Rendering_SetMultiViewEx(starvr::MV_Mode mode = MV_NONE);
}