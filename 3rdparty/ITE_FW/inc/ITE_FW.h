#pragma once

#include "ITE_FW_common.h"

#include <stdint.h>

#if defined(_WIN32)
#define ITE_DLL_EXPORT extern "C" __declspec( dllexport )
#define ITE_DLL_EXPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define ITE_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define ITE_DLL_EXPORT extern "C" 
#else
#error "Unsupported Platform."
#endif

ITE_DLL_EXPORT int ITE_ConnectToHMD(void* handle);
ITE_DLL_EXPORT int ITE_DisconnectHMD(void* handle);

ITE_DLL_EXPORT int ITE_BrightnessRead(void*, uint8_t* brightnessOut);
ITE_DLL_EXPORT int ITE_BrightnessWrite(void*, DisplayPanel panel, uint8_t brightness);

ITE_DLL_EXPORT int ITE_FPSSettingRead(void*, DisplayRefreshRate* rate);
ITE_DLL_EXPORT int ITE_FPSSettingWrite(void*, DisplayRefreshRate rate);