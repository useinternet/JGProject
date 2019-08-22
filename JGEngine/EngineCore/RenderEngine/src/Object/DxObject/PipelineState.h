#pragma once

#include "DxObject.h"


namespace RE
{
	class Shader;
	class RootSignature;
	class PipelineState : public DxObject
	{
	public:
		PipelineState();
		PipelineState(const std::string& name);

		bool IsVaild() const;
		ID3D12PipelineState* GetD3DPipelineState() const;
	protected:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_D3D_PipelineState;
	};

	class GraphicsPipelineState : public PipelineState
	{
	public:
		GraphicsPipelineState();
		GraphicsPipelineState(const std::string& name);


		void BindShader(const Shader& shader);
		void SetInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& intput_element_desc);
		void SetRootSignature(const RootSignature& rootsig);
		void SetRenderTargetFormat(const std::vector<uint32_t>& slot, const std::vector<DXGI_FORMAT>& formats);
		void SetDepthStencilFormat(DXGI_FORMAT format);

		void SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
		void SetBlendState(const D3D12_BLEND_DESC& desc);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);
		void SetSampleMask(uint32_t mask);
		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
		void SetSampleDesc(uint32_t count, uint32_t quality);

		bool Finalize();

	private:
		void Init();
		D3D12_GRAPHICS_PIPELINE_STATE_DESC    m_Desc;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElementDesc;
		ComPtr<ID3D12RootSignature>           m_RootSignature;
	};


	class ComputePipelineState : public PipelineState
	{
	private:
		D3D12_COMPUTE_PIPELINE_STATE_DESC m_Desc;
	};
}