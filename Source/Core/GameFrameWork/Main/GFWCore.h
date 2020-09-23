#pragma once


#ifdef GAMEFRAMEWORK_EXPORT

#define GAMEFRAMEWORK_API __declspec(dllexport)

#else

#define GAMEFRAMEWORK_API __declspec(dllimport)

#endif
#include "CommonCore.h"
#include "GraphicsIF.h"

//