#pragma once
#pragma warning(disable : 4251 4275 26495 26451 6387 26439 26498)

#ifdef INPUTENGINE_EXPORTS

#define INPUTEGINE_API __declspec(dllexport)

#else

#define INPUTEGINE_API __declspec(dllimport)

#endif

#include "CommonCore.h"