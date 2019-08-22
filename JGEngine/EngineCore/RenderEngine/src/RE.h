#pragma once


#ifdef RENDERENGINE_EXPORTS

#define RENDERENGINE_API  __declspec(dllexport)

#else

#define RENDERENGINE_API  __declspec(dllimport)

#endif

#include "CommonCore.h"


