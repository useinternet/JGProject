#pragma once

#include "DirectX2DCommon/DirectX2DCommon.h"


namespace Dx2D
{
	namespace Dx2DDevice
	{
		bool CreateDevice(ID3D12Device* dx12device, ComPtr<ID3D12CommandQueue> cmdQueue, const std::string& logpath = DX2D_LOG_PATH);
		void DestoryDevice();
		ID2D1Factory* GetDx2DFactory();
		IDWriteFactory* GetDxWriteFactory();
		float GetDpiX();
		float GetDpiY();
	}
}