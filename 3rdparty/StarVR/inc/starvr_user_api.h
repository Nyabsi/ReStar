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
	STARVR_DLL_EXPORT StarVRError StarVR_User_SetIPD(float ipd);
	STARVR_DLL_EXPORT StarVRError StarVR_User_GetIPD(float* ipd);
}