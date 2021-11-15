#pragma once


#ifndef GAME_API
#ifdef GAME_DLL_EXPORT
#define GAME_API extern "C" __declspec(dllexport)
#else
#define GAME_API extern "C" __declspec(dllimport)
#endif
#endif 

#include "JGCore.h"



GAME_API void TestFunction();