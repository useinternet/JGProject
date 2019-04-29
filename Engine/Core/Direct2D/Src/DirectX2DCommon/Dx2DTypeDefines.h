#pragma once
#include"DirectX2DCommon.h"


namespace Dx2D
{
	extern std::string g_LogGroupName;

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
}

