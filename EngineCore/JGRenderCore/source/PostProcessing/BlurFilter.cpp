#include "BlurFilter.h"
#include "DxCore/DxCore.h"
#include"Shader/BlurShaderRootSignature.h"
#include"Shader/Shader.h"
using namespace JGRC;
using namespace std;
using namespace Microsoft::WRL;

BlurFilter::BlurFilter(DxCore* core, ResourceManager* Manager, UINT width, UINT height, DXGI_FORMAT format,
	const std::wstring& shaderPath)
{
	m_DxCore = core;
	m_ResourceManager = Manager;
	m_Width = width;
	m_Height = height;
	m_Format = format;
	m_RootSig = make_unique<BlurShaderRootSignature>();
	m_RootSig->RootSign(core->Device());
	m_Shader = make_unique<ComputeShader>();
	m_Shader->Init(m_DxCore, m_RootSig.get(), shaderPath, { "VertBlurCS","HorzBlurCS" });

	ZeroMemory(&m_ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	BuildResource();
	BuildDescriptors();
}
BlurFilter::~BlurFilter()
{

}
void BlurFilter::OnReSize(UINT newWidth, UINT newHeight)
{
	if ((m_Width != newWidth) || (m_Height != newHeight))
	{
		m_Width = newWidth;
		m_Height = newHeight;

		m_ResourceDesc.Width = m_Width;
		m_ResourceDesc.Height = m_Height;
		m_BlurMap0 = m_ResourceManager->ReBuildResource(m_BlurMap0, &m_ResourceDesc);
		m_BlurMap1 = m_ResourceManager->ReBuildResource(m_BlurMap1, &m_ResourceDesc);
		m_ResourceManager->SetSrv("SrvBlurMap0", m_BlurMap0);
		m_ResourceManager->SetUav("UavBlurMap0", m_BlurMap0, nullptr);
		m_ResourceManager->SetSrv("SrvBlurMap1", m_BlurMap1);
		m_ResourceManager->SetUav("UavBlurMap1", m_BlurMap1, nullptr);
	}
}
void BlurFilter::Execute(ID3D12Resource* input, int blurCount)
{
	static float sigma = 0.0f;
	sigma += 0.003f;
	auto weights = CalcGaussWeights(sigma);
	int blurRadius = (int)weights.size() / 2;

	auto CommandList = m_DxCore->CommandList();
	CommandList->SetComputeRootSignature(m_RootSig->GetRootSignature());
	CommandList->SetComputeRoot32BitConstants(0, 1, &blurRadius, 0);
	CommandList->SetComputeRoot32BitConstants(0, (UINT)weights.size(), weights.data(), 1);

	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BlurMap0,
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	// BlurMap0 에 현재 백버퍼에 그린 이미지 복제
	CommandList->CopyResource(m_BlurMap0, input);

	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BlurMap0,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BlurMap1,
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	for (int i = 0; i < blurCount; ++i)
	{
		// 수평 블러
		CommandList->SetPipelineState(m_Shader->GetPSO("HorzBlurCS"));
		UINT Blur0GpuSrvIndex = m_ResourceManager->GetSrvUavIndex("SrvBlurMap0");
		UINT Blur0GpuUavIndex = m_ResourceManager->GetSrvUavIndex("UavBlurMap0");
		UINT Blur1GpuSrvIndex = m_ResourceManager->GetSrvUavIndex("SrvBlurMap1");
		UINT Blur1GpuUavIndex = m_ResourceManager->GetSrvUavIndex("UavBlurMap1");

		CommandList->SetComputeRootDescriptorTable(
			(UINT)EBlurShaderSlot::srvInput,
			m_ResourceManager->GetGPUSrvUavHandle(Blur0GpuSrvIndex));
		CommandList->SetComputeRootDescriptorTable(
			(UINT)EBlurShaderSlot::uavOutput,
			m_ResourceManager->GetGPUSrvUavHandle(Blur1GpuUavIndex));

		UINT numGroupsX = (UINT)ceilf(m_Width / 256.0f);
		CommandList->Dispatch(numGroupsX, m_Height, 1);

		CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BlurMap0,
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
		CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BlurMap1,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
		// 수직 블러
		CommandList->SetPipelineState(m_Shader->GetPSO("VertBlurCS"));


		CommandList->SetComputeRootDescriptorTable(
			(UINT)EBlurShaderSlot::srvInput,
			m_ResourceManager->GetGPUSrvUavHandle(Blur1GpuSrvIndex));
		CommandList->SetComputeRootDescriptorTable(
			(UINT)EBlurShaderSlot::uavOutput,
			m_ResourceManager->GetGPUSrvUavHandle(Blur0GpuUavIndex));
		
		UINT numGroupsY = (UINT)ceilf(m_Height / 256.0f);
		CommandList->Dispatch(m_Width, numGroupsY, 1);
		CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BlurMap0,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
		CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BlurMap1,
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
	}

	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BlurMap0,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COMMON));
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BlurMap1,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON));

	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
	CommandList->CopyResource(input, m_BlurMap0);
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET));
	// 

}
ID3D12Resource* BlurFilter::Output()
{
	return m_BlurMap0;
}
void BlurFilter::BuildDescriptors()
{
	m_SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	m_SrvDesc.Format = m_Format;
	m_SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	m_SrvDesc.Texture2D.MostDetailedMip = 0;
	m_SrvDesc.Texture2D.MipLevels = 1;

	m_UavDesc.Format = m_Format;
	m_UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	m_UavDesc.Texture2D.MipSlice = 0;


	m_ResourceManager->AddSrv("SrvBlurMap0", m_BlurMap0, &m_SrvDesc);
	m_ResourceManager->AddUav("UavBlurMap0", m_BlurMap0, nullptr, &m_UavDesc);
	m_ResourceManager->AddSrv("SrvBlurMap1", m_BlurMap1, &m_SrvDesc);
	m_ResourceManager->AddUav("UavBlurMap1", m_BlurMap1, nullptr, &m_UavDesc);
}
void BlurFilter::BuildResource()
{
	m_ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_ResourceDesc.Alignment = 0;
	m_ResourceDesc.Width  = m_Width;
	m_ResourceDesc.Height = m_Height;
	m_ResourceDesc.DepthOrArraySize = 1;
	m_ResourceDesc.MipLevels = 1;
	m_ResourceDesc.Format = m_Format;
	m_ResourceDesc.SampleDesc.Count = 1;
	m_ResourceDesc.SampleDesc.Quality = 0;
	m_ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	m_BlurMap0 = m_ResourceManager->BuildResource(&m_ResourceDesc);
	m_BlurMap1 = m_ResourceManager->BuildResource(&m_ResourceDesc);
}
vector<float> BlurFilter::CalcGaussWeights(float sigma)
{
	float twoSigma2 = 2.0f*sigma*sigma;

	int blurRadius = (int)ceil(2.0f * sigma);

	if (blurRadius >= MaxBlurRadius)
		blurRadius = MaxBlurRadius;


	std::vector<float> weights;
	weights.resize(2 * blurRadius + 1);

	float weightSum = 0.0f;
	for (int i = -blurRadius; i <= blurRadius; ++i)
	{
		float x = (float)i;
		weights[i + blurRadius] = expf(-x * x / twoSigma2);

		weightSum += weights[i + blurRadius];
	}

	// Divide by the sum so all the weights add up to 1.0.
	for (int i = 0; i < weights.size(); ++i)
	{
		weights[i] /= weightSum;
	}

	return weights;
}