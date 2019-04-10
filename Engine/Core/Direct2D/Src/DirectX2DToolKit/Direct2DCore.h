#pragma once

#include "DirectX2DCommon/DirectX2DCommon.h"


namespace Dx2D
{
	namespace Dx2DDevice
	{
		bool CreateDevice(const std::string& logpath = DX2D_LOG_PATH);
		void DestoryDevice();
		ID2D1Factory* GetDx2DFactory();
		IDWriteFactory* GetDxWriteFactory();
	}
}