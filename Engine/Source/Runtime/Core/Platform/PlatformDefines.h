#pragma once


// �ӽ� �⺻ Window
#define _JG_PLATFORM_WINDOWS

// Platform �� Include
// Window
#ifdef _JG_PLATFORM_WINDOWS
#include <Windows.h>
#endif

// Platform �� Define

#ifdef _JG_PLATFORM_WINDOWS

using HJModule   = HMODULE;
using HJInstance = HINSTANCE;
using HJWHandle  = HWND;
#endif