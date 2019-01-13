#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	struct PipeLineStateSetDesc
	{
		UINT SampleMask = UINT_MAX;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		UINT NumRenderTargets = 1;
		DXGI_FORMAT RTVFormats[8] = { DXGI_FORMAT_R8G8B8A8_UNORM, };
		UINT SampleDescCount = 1;
		UINT SampleDescQuality = 0;
		DXGI_FORMAT DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC Get() const
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			ZeroMemory(&desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
			desc.SampleMask = SampleMask;
			desc.PrimitiveTopologyType = PrimitiveType;
			desc.NumRenderTargets = NumRenderTargets;
			for (UINT i = 0; i < NumRenderTargets; ++i)
				desc.RTVFormats[i] = RTVFormats[i];
			desc.SampleDesc.Count   = SampleDescCount;
			desc.SampleDesc.Quality = SampleDescQuality;
			desc.DSVFormat = DSVFormat;
			return desc;
		}
		D3D12_GRAPHICS_PIPELINE_STATE_DESC UnUsedRenderTarget() 
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = Get();
			desc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
			desc.NumRenderTargets = 0;
			return desc;
		}
		D3D12_GRAPHICS_PIPELINE_STATE_DESC SSAONormalRenderTarget()
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = Get();
			desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			return desc;
		}
		D3D12_GRAPHICS_PIPELINE_STATE_DESC SSAORenderTarget()
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = Get();
			desc.RTVFormats[0] = DXGI_FORMAT_R16_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.DSVFormat = DXGI_FORMAT_UNKNOWN;
			return desc;
		}
	};
	class RCORE_EXPORT BSDescList
	{
	private:
		BSDescList()  = delete;
		~BSDescList() = delete;
	public:
		struct RT_BSDesc
		{
			UINT Slot;
			D3D12_RENDER_TARGET_BLEND_DESC Desc;
		};
		static D3D12_RENDER_TARGET_BLEND_DESC AddAlpha()
		{
			D3D12_RENDER_TARGET_BLEND_DESC Desc;
			Desc.BlendEnable = true;
			Desc.LogicOpEnable = false;
			Desc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			Desc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			Desc.BlendOp = D3D12_BLEND_OP_ADD;
			Desc.SrcBlendAlpha = D3D12_BLEND_ONE;
			Desc.DestBlendAlpha = D3D12_BLEND_ZERO;
			Desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			Desc.LogicOp = D3D12_LOGIC_OP_NOOP;
			Desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			return Desc;
		}
		static D3D12_BLEND_DESC Default()
		{
			return CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		}
		static D3D12_BLEND_DESC Custom(const std::vector<RT_BSDesc>& RenderTargetIndex, bool AlphaToCoverageEnable = false,bool IndependentBlendEnable = false)
		{
			D3D12_BLEND_DESC BlendDesc = Default();
			BlendDesc.AlphaToCoverageEnable  = AlphaToCoverageEnable;
			BlendDesc.IndependentBlendEnable = IndependentBlendEnable;
			for (auto& desc : RenderTargetIndex)
				BlendDesc.RenderTarget[desc.Slot] = desc.Desc;
			return BlendDesc;
		}
	};
	class RCORE_EXPORT RSDescList
	{
	private:
		RSDescList() = delete;
		~RSDescList() = delete;
	public:
		static D3D12_RASTERIZER_DESC Default()
		{
			return CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		}
		static D3D12_RASTERIZER_DESC WireFrame()
		{
			D3D12_RASTERIZER_DESC Desc = Default();
			Desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
			return Desc;
		}
		static D3D12_RASTERIZER_DESC CullNone()
		{
			D3D12_RASTERIZER_DESC Desc = Default();
			Desc.CullMode = D3D12_CULL_MODE_NONE;
			return Desc;
		}
		static D3D12_RASTERIZER_DESC ShadowBias()
		{
			D3D12_RASTERIZER_DESC Desc = Default();
			Desc.DepthBias = 80000;
			Desc.DepthBiasClamp = 0.0f;
			Desc.SlopeScaledDepthBias = 1.0f;
			return Desc;
		}
	};

	class RCORE_EXPORT DSSDescList
	{
	private:
		DSSDescList() = delete;
		~DSSDescList() = delete;
	public:
		static D3D12_DEPTH_STENCIL_DESC Default()
		{
			return CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		}
		static D3D12_DEPTH_STENCIL_DESC LessEqual()
		{
			D3D12_DEPTH_STENCIL_DESC Desc = Default();
			Desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			return Desc;
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