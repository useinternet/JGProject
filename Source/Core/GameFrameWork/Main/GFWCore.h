#pragma once


#ifdef GAMEFRAMEWORK_EXPORT

#define GFW_API __declspec(dllexport)

#else

#define GFW_API __declspec(dllimport)

#endif
//#include "CommonCore.h"
