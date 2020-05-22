#pragma once
#ifdef GRAPHICSENGINE_EXPORTS

#define GRAPHICSENGINE_API  __declspec(dllexport)

#else

#define GRAPHICSENGINE_API  __declspec(dllimport)

#endif

#include <CommonCore.h>

#define GE_LOG(level, str, ...) LOG(level, Graphics, str, __VA_ARGS__)




namespace GE
{
	enum _FORMAT
	{
		FORMAT_R8G8B8A8_UNORM





	};

	enum _ScreenFlags
	{
		ScreenFlag_None = 0


	};
}
