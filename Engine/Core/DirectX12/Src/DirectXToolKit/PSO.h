#pragma once

#include"DirectXCommon/DirectXCommon.h"

namespace Dx12
{
	class Shader;
	class GraphicsShader;
	class ComputeShader;
	class PSO
	{
	protected:
		ID3D12RootSignature* m_RootSig = nullptr;
		ID3D12PipelineState* m_PSO = nullptr;
	public:
		PSO()  = default;
		virtual ~PSO() = default;

		static void DestroyPSO();
	public:
		void SetRootSig(ID3D12RootSignature* rootsig) {
			m_RootSig = rootsig;
		}
		ID3D12RootSignature* GetRootSig() const
		{
			return m_RootSig;
		}
		ID3D12PipelineState* Get() const
		{
			return m_PSO;
		}
	};

	class GraphicsPSO : public PSO
	{
	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_PSODesc;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayouts;
	public:
		GraphicsPSO();

		void SetBlendState(const D3D12_BLEND_DESC& BlendDesc);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc);
		void SetSampleMask(UINT SampleMask);
		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType);
		void SetRenderTargetFormat(DXGI_FORMAT RTVFormat, DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
		void SetRenderTargetFormats(UINT NumRTVs, const DXGI_FORMAT* RTVFormats, DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
		void SetInputLayout(UINT NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs);
		void SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBProps);

		void SetVertexShader(const void* Binary, size_t Size)   { m_PSODesc.VS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
		void SetPixelShader(const void* Binary, size_t Size)    { m_PSODesc.PS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
		void SetGeometryShader(const void* Binary, size_t Size) { m_PSODesc.GS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
		void SetHullShader(const void* Binary, size_t Size)     { m_PSODesc.HS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
		void SetDomainShader(const void* Binary, size_t Size)   { m_PSODesc.DS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }

		void SetVertexShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.VS = Binary; }
		void SetPixelShader(const D3D12_SHADER_BYTECODE& Binary)  { m_PSODesc.PS = Binary; }
		void SetGeometryShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.GS = Binary; }
		void SetHullShader(const D3D12_SHADER_BYTECODE& Binary)   { m_PSODesc.HS = Binary; }
		void SetDomainShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.DS = Binary; }

		void SetVertexShader(const GraphicsShader& shader);
		void SetPixelShader(const GraphicsShader& shader);
		void SetGeometryShader(const GraphicsShader& shader);
		void SetHullShader(const GraphicsShader& shader);
		void SetDomainShader(const GraphicsShader& shader);

		void Finalize();
	};


	class ComputePSO : public PSO
	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC m_PSODesc;
	public:
		ComputePSO();

		void SetComputeShader(const void* Binary, size_t Size) { m_PSODesc.CS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
		void SetComputeShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.CS = Binary; }

		void Finalize();
	};

}