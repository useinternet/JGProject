#include "pch.h"
#include "PipelineState.h"
#include "Shader.h"
#include "RootSignature.h"
#include "Object/ReObject/RenderTarget.h"
using namespace std;
namespace RE
{
	PipelineState::PipelineState() : DxObject("PipelineState")
	{

	}
	PipelineState::PipelineState(const std::string& name) : 
		DxObject(name)
	{

	}
	bool PipelineState::IsVaild() const
	{
		return m_D3D_PipelineState.Get() != nullptr;
	}
	ID3D12PipelineState* PipelineState::GetD3DPipelineState() const {
		return m_D3D_PipelineState.Get();
	}


	GraphicsPipelineState::GraphicsPipelineState() : 
		PipelineState("GraphicsPipelineState")
	{
		Init();
	}
	GraphicsPipelineState::GraphicsPipelineState(const std::string& name) : 
		PipelineState(name)
	{
		Init();
	}

	void GraphicsPipelineState::BindShader(const Shader& shader)
	{
		
		switch (shader.GetType())
		{
		case ShaderType::Vertex:
			if (m_Desc.VS.pShaderBytecode)
				RE_LOG_ERROR("{0} is already bound to the vertex shader.Therefore this shader is ignored.", GetName());
			m_Desc.VS.pShaderBytecode = shader.GetShaderCode()->GetBufferPointer();
			m_Desc.VS.BytecodeLength = shader.GetShaderCode()->GetBufferSize();
			break;
		case ShaderType::Hull:
			if (m_Desc.HS.pShaderBytecode)
				RE_LOG_ERROR("{0} is already bound to the hull shader.Therefore this shader is ignored.", GetName());
			m_Desc.HS.pShaderBytecode = shader.GetShaderCode()->GetBufferPointer();
			m_Desc.HS.BytecodeLength = shader.GetShaderCode()->GetBufferSize();
			break;
		case ShaderType::Domain:
			if (m_Desc.DS.pShaderBytecode)
				RE_LOG_ERROR("{0} is already bound to the domain shader.Therefore this shader is ignored.", GetName());
			m_Desc.DS.pShaderBytecode = shader.GetShaderCode()->GetBufferPointer();
			m_Desc.DS.BytecodeLength = shader.GetShaderCode()->GetBufferSize();
			break;
		case ShaderType::Geometry:
			if (m_Desc.GS.pShaderBytecode)
				RE_LOG_ERROR("{0} is already bound to the geometry shader.Therefore this shader is ignored.", GetName());
			m_Desc.GS.pShaderBytecode = shader.GetShaderCode()->GetBufferPointer();
			m_Desc.GS.BytecodeLength = shader.GetShaderCode()->GetBufferSize();
			break;
		case ShaderType::Pixel:
			if (m_Desc.PS.pShaderBytecode)
				RE_LOG_ERROR("{0} is already bound to the pixel shader.Therefore this shader is ignored.", GetName());
			m_Desc.PS.pShaderBytecode = shader.GetShaderCode()->GetBufferPointer();
			m_Desc.PS.BytecodeLength = shader.GetShaderCode()->GetBufferSize();
			break;
		case ShaderType::Compute:
			RE_LOG_ERROR("{0} is already bound to the compute shader.Therefore this shader is ignored.", shader.GetName());
			break;
		}
	}
	void GraphicsPipelineState::SetInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& intput_element_desc)
	{
		m_InputElementDesc = std::move(intput_element_desc);
		m_Desc.InputLayout.NumElements = (uint32_t)m_InputElementDesc.size();
		m_Desc.InputLayout.pInputElementDescs = m_InputElementDesc.data();
	}
	void GraphicsPipelineState::SetRootSignature(const RootSignature& rootsig)
	{
		m_RootSignature = rootsig.GetD3DRootSignature();
		m_Desc.pRootSignature = m_RootSignature.Get();
	}
	void GraphicsPipelineState::SetRenderTargetFormat(const std::vector<uint32_t>& slot, const std::vector<DXGI_FORMAT>& formats)
	{
		if (slot.size() != formats.size())
		{
			RE_LOG_ERROR("{0} :: BindRenderTarget does not match slot number and format number", GetName());
		}
		uint32_t numRenderTarget = (uint32_t)slot.size();
	
		for (uint32_t i = 0; i < numRenderTarget; ++i)
		{
			uint32_t num = slot[i];
			if (num >= 8)
			{
				RE_LOG_ERROR("{0} ::BindRenderTarget slot cannot exceed 7", GetName());
				continue;
			}
			m_Desc.RTVFormats[num] = formats[i];
			m_Desc.NumRenderTargets++;
		}
	}
	void GraphicsPipelineState::SetDepthStencilFormat(DXGI_FORMAT format)
	{
		m_Desc.DSVFormat = format;
	}
	void GraphicsPipelineState::BindRenderTarget(const RenderTarget& rt)
	{
		std::vector<uint32_t> slots;
		std::vector<DXGI_FORMAT> formats;

		for (uint32_t i = 0; RenderTarget::GetMaxNumRenderTarget(); ++i)
		{
			auto t = rt.GetTexture(i);
			if (t.IsVaild())
			{
				slots.push_back(i);
				formats.push_back(t.GetDesc().Format);
			}
			else
				break;
		}

		SetRenderTargetFormat(slots, formats);
		auto t = rt.GetDepthTexture();
		if (t.IsVaild())
		{
			SetDepthStencilFormat(t.GetDesc().Format);

		}

	}
	void GraphicsPipelineState::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
	{
		m_Desc.RasterizerState = desc;
	}
	void GraphicsPipelineState::SetBlendState(const D3D12_BLEND_DESC& desc)
	{
		m_Desc.BlendState = desc;
	}
	void GraphicsPipelineState::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
	{
		m_Desc.DepthStencilState = desc;
	}
	void GraphicsPipelineState::SetSampleMask(uint32_t mask)
	{
		m_Desc.SampleMask = mask;
	}
	void GraphicsPipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
	{
		m_Desc.PrimitiveTopologyType = type;
	}
	void GraphicsPipelineState::SetSampleDesc(uint32_t count, uint32_t quality)
	{
		m_Desc.SampleDesc.Count = count;
		m_Desc.SampleDesc.Quality = quality;
	}
	void GraphicsPipelineState::Init()
	{
		ZeroMemory(&m_Desc, sizeof(m_Desc));
		m_Desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		m_Desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		m_Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		m_Desc.SampleMask = UINT_MAX;
		m_Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_Desc.SampleDesc.Count = 1;
		m_Desc.SampleDesc.Quality = 0;
	}

	bool GraphicsPipelineState::Finalize()
	{
		if (!m_RootSignature)
		{
			RE_LOG_ERROR("{0}::Finalize RootSignature is nullptr", GetName());
			return false;
		}
		if (m_InputElementDesc.empty())
		{
			RE_LOG_ERROR("{0}::Finalize InputElementDescs is nullptr", GetName());
			return false;
		}


		HRESULT hr = GetD3DDevice()->CreateGraphicsPipelineState(
			&m_Desc, IID_PPV_ARGS(m_D3D_PipelineState.GetAddressOf()));
		if (FAILED(hr))
		{
			RE_LOG_ERROR("{0}::Finalize PipelineState failed Create D3DPipelineState.", GetName());
			return false;
		}
		return true;
	}
}