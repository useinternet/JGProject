#pragma once

#ifdef _JGEDITOR
#define JGEDITOR_API __declspec(dllexport)
#define JGEDITOR_C_API extern "C" __declspec(dllexport)
#else
#define JGEDITOR_API __declspec(dllimport)
#define JGEDITOR_C_API extern "C" __declspec(dllimport)
#endif

