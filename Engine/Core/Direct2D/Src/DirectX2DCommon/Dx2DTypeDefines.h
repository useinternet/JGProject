#pragma once
#include"DirectX2DCommon.h"


namespace Dx2D
{
	namespace Dx2DDevice
	{
		extern std::string g_LogGroupName;
	}

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
}

