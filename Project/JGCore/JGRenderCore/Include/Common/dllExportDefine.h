#pragma once
#ifdef JGRENDERCORE_EXPORTS 
#define CORE_EXPORT __declspec(dllexport)
#else
#define CORE_EXPORT __declspec(dllimport)
#endif

#pragma warning (disable : 4996 4005 4251)