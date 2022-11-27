#pragma once


// 임시 기본 Window
#define _JG_PLATFORM_WINDOWS

// Platform 별 Include
// Window
#ifdef _JG_PLATFORM_WINDOWS
#include <Windows.h>
#endif

// Platform 별 Define

#ifdef _JG_PLATFORM_WINDOWS

using HJModule   = HMODULE;
using HJInstance = HINSTANCE;
using HJWHandle  = HWND;
#endif