#pragma once


#pragma warning(disable : 4251 4275 26495 26451 6387 26439 26498)

#ifdef JGENGINE_EXPORTS

#define JGENGINE_API __declspec(dllexport)

#else

#define JGENGINE_API __declspec(dllimport)

#endif


#include "CommonCore.h"
