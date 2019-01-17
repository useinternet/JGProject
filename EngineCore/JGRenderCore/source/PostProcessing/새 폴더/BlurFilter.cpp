#include"BlurFilter.h"
#include"Shader/BlurShaderRootSignature.h"
#include"Data/CommonData.h"
#include"Shader/Shader.h"
#include"Shader/ShaderPath.h"
#include"Data/Scene.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
UINT BlurFilter::Count = 0;

void BlurFilter::BuildFilter(UINT width, UINT height, SrvResourcePack* OwnerSrvPack,
	RenderTargetPack* OwnerRtvPack)
{
	m_Width = width;
	m_Height = height;
	m_OwnerSrvPack = OwnerSrvPack;
	m_OwnerRtvPack = OwnerRtvPack;
	m_BlurRootSig = make_unique<BlurShaderRootSignature>();
	m_BlurRootSig->RootSign(CommonData::_Device());
	m_BlurShader = make_unique<Shader>();
	m_BlurShader->Init(global_blur_hlsl_pah, { EShaderType::Pixel, EShaderType::Vertex });
	m_BlurPSO = m_BlurShader->CompileAndConstrutPSO(EPSOMode::ONETARGET_DEFAULT,
		m_BlurRootSig.get(), {});
	m_BlurCB = make_unique<UploadBuffer<BlurData>>(CommonData::_Device(), 1, true);
}
void BlurFilter::ReSize(UINT width, UINT height, SrvResourcePack* OwnerSrvPack, RenderTargetPack* OwnerRtvPack)
{
	m_OwnerSrvPack = OwnerSrvPack;
	m_OwnerRtvPack = OwnerRtvPack;
	if (m_Width != width || m_Height != height)
	{
		m_Width = width;
		m_Height = height;
		ReBuildResource();
		ReRegisterDescriptor();
	}
}
void BlurFilter::Update()
{
	cbPassConstant MainPassCB = CommonData::_Scene()->MainPassData();
	BlurData data;
	data.Proj = MainPassCB.Proj;
	data.InvRenderTargetSize = XMFLOAT2(1.0f / m_Width, 1.0f / m_Height / 2);


	auto blurWeight = CalcGaussWeights(2.5f);
	data.BlurWeight[0] = XMFLOAT4(&blurWeight[0]);
	data.BlurWeight[1] = XMFLOAT4(&blurWeight[4]);
	data.BlurWeight[2] = XMFLOAT4(&blurWeight[8]);

	m_BlurCB->CopyData(0, data);
}
void BlurFilter::ExcuteBlur(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, UINT BlurCount)
{
	CommandList->SetPipelineState(m_BlurPSO);
	auto blurcbAddress = m_BlurCB->Resource()->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView((UINT)EBlurShaderSlot::cbBlur,
		blurcbAddress);
	for (UINT i = 0; i < BlurCount; ++i)
	{

	}
	CommonData::_Scene()->SettingDefaultSceneBuffer(CommandList, CurrFrameResource);
}
void BlurFilter::BuildResource()
{
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_RESOURCE_DESC));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = m_Width;
	desc.Height = m_Height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = m_OwnerSrvPack->Desc.Format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	ResourceFlagPack pack;
	pack.State = D3D12_RESOURCE_STATE_GENERIC_READ;


	m_BlurResource = CommonData::_ResourceManager()->BuildResource(
		&desc, pack);
}
void BlurFilter::ReBuildResource()
{
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_RESOURCE_DESC));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = m_Width;
	desc.Height = m_Height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = m_OwnerSrvPack->Desc.Format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	ResourceFlagPack pack;
	pack.State = D3D12_RESOURCE_STATE_GENERIC_READ;

	m_BlurResource = CommonData::_ResourceManager()->ReBuildResource(
		m_BlurResource, &desc, pack);
}
void BlurFilter::RegisterDescriptor()
{
	m_BlurPack = CommonData::_ResourceManager()->AddSrv(
		m_OwnerSrvPack->PackName + "Blur_Copy_Srv_" + to_string(Count), m_BlurResource, &m_OwnerSrvPack->Desc);
	m_BlurRtvPack = CommonData::_ResourceManager()->AddRtv(
		m_OwnerRtvPack->PackName + "Blur_Copy_Rtv_" + to_string(Count++), m_BlurResource, &m_OwnerRtvPack->Desc);
}
void BlurFilter::ReRegisterDescriptor()
{
	m_BlurPack = CommonData::_ResourceManager()->SetSrv(
		m_BlurPack->PackName, m_BlurResource, &m_BlurPack->Desc);

	m_OwnerRtvPack = CommonData::_ResourceManager()->SetRtv(
		m_OwnerRtvPack->PackName, m_BlurResource, &m_OwnerRtvPack->Desc);
}
vector<float> BlurFilter::CalcGaussWeights(float sigma)
{
	float twoSigma2 = 2.0f*sigma*sigma;

	// Estimate the blur radius based on sigma since sigma controls the "width" of the bell curve.
	// For example, for sigma = 3, the width of the bell curve is 
	int blurRadius = (int)ceil(2.0f * sigma);

	if (blurRadius > 5)
		blurRadius = 5;

	vector<float> weights;
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