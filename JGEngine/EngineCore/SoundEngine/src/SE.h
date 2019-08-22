#pragma once


#pragma warning(disable : 4251 4275 26495 26451 6387 26439 26498)

#ifdef SOUNDENGINE_EXPORTS

#define SOUNDENGINE_API __declspec(dllexport)

#else

#define SOUNDENGINE_API __declspec(dllimport)

#endif

#include "CommonCore.h"