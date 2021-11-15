#pragma once


#ifndef EDITOR_API
#ifdef EDITOR_DLL_EXPORT
#define EDITOR_API extern "C" __declspec(dllexport)
#else
#define EDITOR_API extern "C" __declspec(dllimport)
#endif
#endif 