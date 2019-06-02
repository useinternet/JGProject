#pragma once


#include"Dx12Include.h"



namespace GR
{
	namespace Dx12
	{
		class VertexShader;
		class HullShader;
		class DomainShader;
		class GeometryShader;
		class PixelShader;
		class ComputeShader;
		class GraphicsDevice;
		class Commander;
		class RootSignature;
		class PSO
		{
		protected:
			ComPtr<ID3D12PipelineState> m_PSO;
			ComPtr<ID3D12RootSignature> m_RootSig;
		public:
			PSO() = default;
			virtual ~PSO() = default;

			ID3D12PipelineState* GetD3DPipelineState() { return m_PSO.Get(); }
			const ID3D12PipelineState* GetD3DPipelineState() const { return m_PSO.Get(); }
		};


		class GraphicsPSO : public PSO
		{
		private:
			D3D12_GRAPHICS_PIPELINE_STATE_DESC    m_Desc;
			std::shared_ptr<const D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
		public:
			GraphicsPSO();
			virtual ~GraphicsPSO() {}
			void SetBlendState(const D3D12_BLEND_DESC& desc);
			void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);
			void SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
			void SetSampleMask(uint32_t sampleMask);
			void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
			void SetRenderTargetFormats(
				const std::vector<uint32_t>& slots,
				const std::vector<DXGI_FORMAT>& formats,
				DXGI_FORMAT DSVFormat = DXGI_FORMAT_UNKNOWN,
				uint32_t msaaCount = 1,
				uint32_t msaaQuality = 0);
			void SetInputLayout(uint32_t numInputLayout, D3D12_INPUT_ELEMENT_DESC* inputDescs);
			void SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE ibProps);
			void BindVertexShader(const VertexShader& shader);
			void BindHullShader(const HullShader& shader);
			void BindDomainShader(const DomainShader& shader);
			void BindGeometryShader(const GeometryShader& shader);
			void BindPixelShader(const PixelShader& shader);
			void BindRootSignature(ComPtr<ID3D12RootSignature> rootSig);
			void BindRootSignature(RootSignature& rootSig);
			void Finalize();
		};

		class ComputePSO : public PSO
		{
		private:
			D3D12_COMPUTE_PIPELINE_STATE_DESC m_Desc;
		public:
			ComputePSO();
			virtual ~ComputePSO() {}
			void BindRootSignature(ComPtr<ID3D12RootSignature> rootSig);
			void BindComputeShader(const ComputeShader& shader);
			void Finalize();

		};

	}
}
