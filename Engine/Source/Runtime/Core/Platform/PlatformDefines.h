#pragma once


// Platform º° Include
// Window
#ifdef _PLATFORM_WINDOWS
#include <Windows.h>
#endif

// Platform º° Define

#ifdef _PLATFORM_WINDOWS

using HJModule   = HMODULE;
using HJInstance = HINSTANCE;
using HJWHandle  = HWND;
#endif