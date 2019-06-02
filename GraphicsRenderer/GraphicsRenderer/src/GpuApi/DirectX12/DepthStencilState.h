#pragma once
#include"Dx12Include.h"


namespace GR
{
	namespace Dx12
	{
		class DepthStencilState
		{
		public:
			static D3D12_DEPTH_STENCIL_DESC Default() {
				return CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			}
			static D3D12_DEPTH_STENCIL_DESC LessEqual() {
				D3D12_DEPTH_STENCIL_DESC desc = Default();
				desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
				return desc;
			}
			static D3D12_DEPTH_STENCIL_DESC NoneDepth()
			{
				D3D12_DEPTH_STENCIL_DESC Desc = Default();
				Desc.DepthEnable = false;
				Desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
				return Desc;
			}
		};
	}
}