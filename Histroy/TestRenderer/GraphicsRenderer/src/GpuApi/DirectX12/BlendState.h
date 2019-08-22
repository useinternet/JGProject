#pragma once
#include"Dx12Include.h"



namespace GR
{
	class BlendState
	{
	public:
		static  D3D12_BLEND_DESC Default() {
			return CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		}
	};

}