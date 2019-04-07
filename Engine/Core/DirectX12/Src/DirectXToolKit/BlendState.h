#pragma once
#include"DirectXCommon/DirectXCommon.h"

namespace Dx12
{
	class BlendState
	{
	public:
		static  D3D12_BLEND_DESC Default() {
			return CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		}
	};
}