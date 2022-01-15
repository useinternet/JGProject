#pragma once
#include "JGCore.h"
#include "DirectX12Helper.h"
#include "Graphics/InputLayout.h"

namespace JG
{
	class RootSignature;
	class DirectX12GraphicsShader;
	class DirectX12ComputeShader;
	//class 
	class PipelineState
	{
	protected:
		ComPtr<ID3D12PipelineState> mD3DPSO;
		bool mIsDirty = true;
	public:
		virtual ~PipelineState() = default;
	public:
		ID3D12PipelineState* Get() const {
			return mD3DPSO.Get();
		}
		virtual bool Finalize() = 0;
	public:
		static void ClearCache();
		
	};

	class GraphicsPipelineState : public PipelineState
	{
	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC mDesc = {};
		List<D3D12_INPUT_ELEMENT_DESC> mD3DInputLayoutDescs;
	public:
		virtual ~GraphicsPipelineState() = default;
	public:
		GraphicsPipelineState();
		void BindRootSignature(const RootSignature& rootSig);
		void BindRenderTarget(const List< DXGI_FORMAT>& rtFormats, DXGI_FORMAT dvFormat = DXGI_FORMAT_UNKNOWN);
		void BindInputLayout(const InputLayout& inputLayout);
		void BindShader(const DirectX12GraphicsShader& shader);
		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
		void SetSampleMask(uint32_t sampleMask);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
		void SetBlendState(const D3D12_BLEND_DESC& desc);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);

		virtual bool Finalize() override;

		const D3D12_BLEND_DESC& GetBlendDesc() const;
	};

	class ComputePipelineState : public PipelineState
	{
	private:
		D3D12_COMPUTE_PIPELINE_STATE_DESC mDesc = {};
	public:
		virtual ~ComputePipelineState() = default;
	public:
		ComputePipelineState();
		void BindRootSignature(const RootSignature& rootSig);
		void BindShader(const DirectX12ComputeShader& shader);


		virtual bool Finalize() override;
	};
}