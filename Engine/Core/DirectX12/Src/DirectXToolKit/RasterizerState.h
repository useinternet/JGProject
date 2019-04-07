#pragma once
#include"DirectXCommon/DirectXCommon.h"

namespace Dx12
{
	class RasterizerState
	{
	public:
		static  D3D12_RASTERIZER_DESC Default() {
			return CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		}
		static D3D12_RASTERIZER_DESC CullNone() {
			D3D12_RASTERIZER_DESC desc = Default();
			desc.CullMode = D3D12_CULL_MODE_NONE;
			return desc;
		}

	};
}