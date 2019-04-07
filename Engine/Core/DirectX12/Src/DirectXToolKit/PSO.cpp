#include<PCH.h>
#include"PSO.h"
#include<Util/Hash.h>
#include"DirectXCore.h"
#include"Shader.h"
using namespace Dx12;
using namespace std;
using namespace Common;
static map<size_t, ComPtr<ID3D12PipelineState>> g_GrpahicPSOHashMap;
static map<size_t, ComPtr<ID3D12PipelineState>> g_ComputePSOHashMap;


void PSO::DestroyPSO()
{
	g_GrpahicPSOHashMap.clear();
	g_ComputePSOHashMap.clear();
}

GraphicsPSO::GraphicsPSO()
{
	ZeroMemory(&m_PSODesc, sizeof(m_PSODesc));
	m_PSODesc.NodeMask = 0;
	m_PSODesc.SampleMask = 0xFFFFFFFFu;
	m_PSODesc.SampleDesc.Count = 1;
	m_PSODesc.InputLayout.NumElements = 0;
	m_PSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	m_PSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	m_PSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
}

void GraphicsPSO::SetBlendState(const D3D12_BLEND_DESC& BlendDesc)
{
	m_PSODesc.BlendState = BlendDesc;
}
void GraphicsPSO::SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc)
{
	m_PSODesc.RasterizerState = RasterizerDesc;
}
void GraphicsPSO::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc)
{
	m_PSODesc.DepthStencilState = DepthStencilDesc;
}
void GraphicsPSO::SetSampleMask(UINT SampleMask)
{
	m_PSODesc.SampleMask = SampleMask;
}
void GraphicsPSO::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType)
{
	m_PSODesc.PrimitiveTopologyType = TopologyType;
}
void GraphicsPSO::SetRenderTargetFormat(DXGI_FORMAT RTVFormat, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
	SetRenderTargetFormats(1, &RTVFormat, DSVFormat, MsaaCount, MsaaQuality);
}
void GraphicsPSO::SetRenderTargetFormats(UINT NumRTVs, const DXGI_FORMAT* RTVFormats, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
	assert((NumRTVs == 0 || RTVFormats != nullptr) && "Null format array conflicts with non-zero length");
	for (UINT i = 0; i < NumRTVs; ++i)
		m_PSODesc.RTVFormats[i] = RTVFormats[i];
	for (UINT i = NumRTVs; i < m_PSODesc.NumRenderTargets; ++i)
		m_PSODesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
	m_PSODesc.NumRenderTargets = NumRTVs;
	m_PSODesc.DSVFormat = DSVFormat;
	m_PSODesc.SampleDesc.Count = MsaaCount;
	m_PSODesc.SampleDesc.Quality = MsaaQuality;
}
void GraphicsPSO::SetInputLayout(UINT NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs)
{
	m_PSODesc.InputLayout.NumElements = NumElements;
	m_InputLayouts.clear();
	if (NumElements > 0)
	{
		for (UINT i = 0; i < NumElements; ++i)
			m_InputLayouts.push_back(pInputElementDescs[i]);
	}
}
void GraphicsPSO::SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBProps)
{
	m_PSODesc.IBStripCutValue = IBProps;
}
void GraphicsPSO::SetVertexShader(const GraphicsShader& shader)
{
	SetVertexShader(shader.GetShaderCode(ShaderStage::Vertex), shader.GetShaderCodeSize(ShaderStage::Vertex));
}
void GraphicsPSO::SetPixelShader(const GraphicsShader& shader)
{
	SetPixelShader(shader.GetShaderCode(ShaderStage::Pixel), shader.GetShaderCodeSize(ShaderStage::Pixel));
}
void GraphicsPSO::SetGeometryShader(const GraphicsShader& shader)
{
	SetGeometryShader(shader.GetShaderCode(ShaderStage::Geometry), shader.GetShaderCodeSize(ShaderStage::Geometry));
}
void GraphicsPSO::SetHullShader(const GraphicsShader& shader)
{
	SetHullShader(shader.GetShaderCode(ShaderStage::Hull), shader.GetShaderCodeSize(ShaderStage::Hull));
}
void GraphicsPSO::SetDomainShader(const GraphicsShader& shader)
{
	SetDomainShader(shader.GetShaderCode(ShaderStage::Domain), shader.GetShaderCodeSize(ShaderStage::Domain));
}
void GraphicsPSO::Finalize()
{
	m_PSODesc.pRootSignature = m_RootSig;
	assert(m_PSODesc.pRootSignature != nullptr);

	m_PSODesc.InputLayout.pInputElementDescs = nullptr;
	size_t HashCode = Common::Util::HashState(&m_PSODesc);
	HashCode = Util::HashState(m_InputLayouts.data(), m_PSODesc.InputLayout.NumElements, HashCode);
	m_PSODesc.InputLayout.pInputElementDescs = m_InputLayouts.data();

	ID3D12PipelineState** PSORef = nullptr;
	bool firstCompile = false;
	{
		static mutex s_HashMapMutex;
		lock_guard<mutex> CS(s_HashMapMutex);
		auto iter = g_GrpahicPSOHashMap.find(HashCode);


		if (iter == g_GrpahicPSOHashMap.end())
		{
			firstCompile = true;
			PSORef = g_GrpahicPSOHashMap[HashCode].GetAddressOf();
		}
		else
			PSORef = iter->second.GetAddressOf();
	}

	if (firstCompile)
	{
		ThrowIfFailed(DxDevice::GetDevice()->CreateGraphicsPipelineState(&m_PSODesc, IID_PPV_ARGS(&m_PSO)));
		g_GrpahicPSOHashMap[HashCode].Attach(m_PSO);
	}
	else
	{
		while (*PSORef == nullptr)
			this_thread::yield();
		m_PSO = *PSORef;
	}

}
ComputePSO::ComputePSO()
{
	ZeroMemory(&m_PSODesc, sizeof(m_PSODesc));
	m_PSODesc.NodeMask = 0;
}
void ComputePSO::Finalize()
{
	m_PSODesc.pRootSignature = m_RootSig;
	assert(m_PSODesc.pRootSignature != nullptr);

	size_t HashCode = Util::HashState(&m_PSODesc);

	ID3D12PipelineState** PSORef = nullptr;
	bool firstCompile = false;
	{
		static mutex s_HashMapMutex;
		lock_guard<mutex> CS(s_HashMapMutex);
		auto iter = g_ComputePSOHashMap.find(HashCode);

	
		if (iter == g_ComputePSOHashMap.end())
		{
			firstCompile = true;
			PSORef = g_ComputePSOHashMap[HashCode].GetAddressOf();
		}
		else
			PSORef = iter->second.GetAddressOf();
	}

	if (firstCompile)
	{
		ThrowIfFailed(DxDevice::GetDevice()->CreateComputePipelineState(&m_PSODesc, IID_PPV_ARGS(&m_PSO)));
		g_ComputePSOHashMap[HashCode].Attach(m_PSO);
	}
	else
	{
		while (*PSORef == nullptr)
			this_thread::yield();
		m_PSO = *PSORef;
	}
}