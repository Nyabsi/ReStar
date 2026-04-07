#pragma once

#include "ITE_FW_common.h"

#include <stdint.h>

#ifndef _ITE_HANDLE
#define _ITE_HANDLE
typedef void* ITE_HANDLE;
#endif // _ITE_HANDLE

#if defined(_WIN32)
#define ITE_DLL_EXPORT extern "C" __declspec( dllexport )
#define ITE_DLL_EXPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define ITE_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define ITE_DLL_EXPORT extern "C" 
#else
#error "Unsupported Platform."
#endif

ITE_DLL_EXPORT int ITE_ConnectToHMD(ITE_HANDLE* handle, int64_t __reserved1, int64_t __reserved2);
ITE_DLL_EXPORT int ITE_DisconnectHMD(ITE_HANDLE handle);

ITE_DLL_EXPORT int ITE_BrightnessRead(ITE_HANDLE handle, uint16_t* brightness);
ITE_DLL_EXPORT int ITE_BrightnessWrite(ITE_HANDLE handle, DisplayPanel panel, uint8_t brightness);

ITE_DLL_EXPORT int ITE_FPSSettingRead(ITE_HANDLE handle, DisplayRefreshRate* rate);
ITE_DLL_EXPORT int ITE_FPSSettingWrite(ITE_HANDLE handle, DisplayRefreshRate rate);