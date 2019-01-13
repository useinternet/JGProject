#include"SSAO.h"
#include"Data/Scene.h"
#include"DxCore/DxCore.h"
#include"Shader/Shader.h"
#include"Shader/CommonShaderRootSignature.h"
#include"Shader/SSAOShaderRootSignature.h"
#include"Data/CommonData.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;
SSAO::SSAO()
{

}
void SSAO::BuildSSAO(UINT width, UINT height, ID3D12GraphicsCommandList* CommandList)
{
	m_Width = width;
	m_Height = height;
	m_SSAOCB = make_unique<UploadBuffer<SSAOData>>(CommonData::_Device(), 1, true);
	
	m_Viewport = { 0.0f,0.0f,(float)m_Width, (float)m_Height,0.0f,1.0f };
	m_ScissorRect = { 0,0,(int)m_Width, (int)m_Height };
	m_SSAOViewport = { 0.0f,0.0f,(float)m_Width / 2, (float)m_Height / 2,0.0f,1.0f };
	m_SSAOScissorRect = { 0,0,(int)m_Width / 2, (int)m_Height / 2 };
	BuildResource(CommandList);
	BuildDescriptor();
}
void SSAO::BuildSSAOShader(CommonShaderRootSignature* RootSig)
{
	m_NormalMapShader = make_unique<Shader>();
	m_NormalMapShader->Init(L"../Contents/Engine/Shaders/DrawNormals.hlsl",
		{ EShaderType::Vertex, EShaderType::Pixel });

	ShaderMacroPack Pack;
	Pack.Macro = SHADER_MACRO_DEFINE_TEXTURE_MAX;
	Pack.Value = to_string(CommonData::_ResourceManager()->GetTextureCount());
	m_NormalMapPSO = m_NormalMapShader->CompileAndConstrutPSO(EPSOMode::SSAO_NORMALMAP,
		RootSig, { Pack });

	m_SSAORootSig = make_unique<SSAOShaderRootSignature>();
	m_SSAORootSig->RootSign(CommonData::_Device());
	m_SSAOShader = make_unique<Shader>();
	m_SSAOShader->Init(L"../Contents/Engine/Shaders/Ssao.hlsl",
		{ EShaderType::Vertex, EShaderType::Pixel });
	m_SSAOPSO = m_SSAOShader->CompileAndConstrutPSO(EPSOMode::SSAO, m_SSAORootSig.get(), {});


	m_SSAOBlurShader = make_unique<Shader>();
	m_SSAOBlurShader->Init(L"../Contents/Engine/Shaders/SsaoBlur.hlsl",
		{ EShaderType::Vertex, EShaderType::Pixel });
	m_SSAOBlurPSO = m_SSAOBlurShader->CompileAndConstrutPSO(EPSOMode::SSAO, m_SSAORootSig.get(), {} );
}
void SSAO::OnReSize(UINT width, UINT height, bool Init)
{
	if (m_Width != width || m_Height != height)
	{
		m_Width = width;
		m_Height = height;
		m_Viewport = { 0.0f,0.0f,(float)m_Width, (float)m_Height,0.0f,1.0f };
		m_ScissorRect = { 0,0,(int)m_Width, (int)m_Height };
		m_SSAOViewport = { 0.0f,0.0f,(float)m_Width / 2, (float)m_Height / 2,0.0f,1.0f };
		m_SSAOScissorRect = { 0,0,(int)m_Width / 2, (int)m_Height / 2 };
		ReBuildResource();
		ReBuildDescrptor();
	}
}
void SSAO::Update(FrameResource* CurrFrameResource)
{
	SSAOData ssaoCB;

	XMMATRIX P = CommonData::_Scene()->GetMainCamera()->GetProj();
	cbPassConstant MainPassCB = CommonData::_Scene()->MainPassData();
	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	ssaoCB.Proj = MainPassCB.Proj;
	ssaoCB.InvProj = MainPassCB.InvProj;
	XMStoreFloat4x4(&ssaoCB.ProjTex, XMMatrixTranspose(P*T));
	for (int i = 0; i < 14; ++i)
	{
		ssaoCB.OffsetVectors[i] = m_Offsets[i];
	}

	auto blurWeights = CalcGaussWeights(2.5f);
	ssaoCB.BlurWeights[0] = XMFLOAT4(&blurWeights[0]);
	ssaoCB.BlurWeights[1] = XMFLOAT4(&blurWeights[4]);
	ssaoCB.BlurWeights[2] = XMFLOAT4(&blurWeights[8]);

	ssaoCB.InvRenderTargetSize = XMFLOAT2(1.0f / (m_Width / 2), 1.0f / (m_Height / 2));

	// Coordinates given in view space.
	ssaoCB.OcclusionRadius = 0.5f;
	ssaoCB.OcclusionFadeStart = 0.2f;
	ssaoCB.OcclusionFadeEnd = 1.0f;
	ssaoCB.SurfaceEpsilon = 0.05f;

	m_SSAOCB->CopyData(0, ssaoCB);
}
void SSAO::Draw(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList)
{
	DrawNormalDepthMap(CurrFrameResource, CommandList);
	DrawSSAO(CurrFrameResource, CommandList);
}
CD3DX12_GPU_DESCRIPTOR_HANDLE SSAO::GetNormalMapGPUHandle()
{
	UINT index = CommonData::_ResourceManager()->GetSrvUavIndex(m_NormalMapPack->PackName);
	return CommonData::_ResourceManager()->GetGPUSrvUavHandle(index);
}
CD3DX12_GPU_DESCRIPTOR_HANDLE SSAO::GetDepthMapGPUHandle()
{
	UINT index = CommonData::_ResourceManager()->GetSrvUavIndex(m_DepthMapPack->PackName);
	return CommonData::_ResourceManager()->GetGPUSrvUavHandle(index);
}
CD3DX12_GPU_DESCRIPTOR_HANDLE SSAO::GetRandomVectorGPUHandle()
{
	UINT index = CommonData::_ResourceManager()->GetSrvUavIndex(m_RandomVectorMapPack->PackName);
	return CommonData::_ResourceManager()->GetGPUSrvUavHandle(index);
}
CD3DX12_GPU_DESCRIPTOR_HANDLE SSAO::GetSSAOMapGPUHandle_0()
{
	UINT index = CommonData::_ResourceManager()->GetSrvUavIndex(m_SSAOMapPack_0->PackName);
	return CommonData::_ResourceManager()->GetGPUSrvUavHandle(index);
}
CD3DX12_GPU_DESCRIPTOR_HANDLE SSAO::GetSSAOMapGPUHandle_1()
{
	UINT index = CommonData::_ResourceManager()->GetSrvUavIndex(m_SSAOMapPack_1->PackName);
	return CommonData::_ResourceManager()->GetGPUSrvUavHandle(index);

}
UINT SSAO::GetShaderIndex()
{
	return CommonData::_ResourceManager()->GetSrvUavIndex(m_SSAOMapPack_0->PackName);
}
void SSAO::BuildResource(ID3D12GraphicsCommandList* CommandList)
{
	ZeroMemory(&m_NormalDesc, sizeof(D3D12_RESOURCE_DESC));
	m_NormalDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_NormalDesc.Alignment = 0;
	m_NormalDesc.Width = m_Width;
	m_NormalDesc.Height = m_Height;
	m_NormalDesc.DepthOrArraySize = 1;
	m_NormalDesc.MipLevels = 1;
	m_NormalDesc.Format = NormalMapFormat;
	m_NormalDesc.SampleDesc.Count = 1;
	m_NormalDesc.SampleDesc.Quality = 0;
	m_NormalDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_NormalDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// ³ë¸Ö ¸Ê ºôµå
	float normalClearColor[] = { 0.0f,0.0f,1.0f,0.0f };
	CD3DX12_CLEAR_VALUE ClearColor(DXGI_FORMAT_R16G16B16A16_FLOAT, normalClearColor);

	ResourceFlagPack FlagPack;
	FlagPack.State = D3D12_RESOURCE_STATE_GENERIC_READ;

	m_NormalMap = CommonData::_ResourceManager()->BuildResource(&m_NormalDesc, FlagPack, &ClearColor);


	// ssao ¸Ê ºôµå
	//
	m_SSAODesc = m_NormalDesc;
	m_SSAODesc.Width = m_Width / 2;
	m_SSAODesc.Height = m_Height / 2;
	m_SSAODesc.Format = SSAOMapFormat;
	float SSAOClearColor[] = { 1.0f,1.0f,1.0f,1.0f };
	ClearColor = CD3DX12_CLEAR_VALUE(SSAOMapFormat, SSAOClearColor);

	m_SSAOMap_0 = CommonData::_ResourceManager()->BuildResource(&m_SSAODesc, FlagPack, &ClearColor);
	m_SSAOMap_1 = CommonData::_ResourceManager()->BuildResource(&m_SSAODesc, FlagPack, &ClearColor);



	// ·£´ý º¤ÅÍ ¸Ê ºôµå
	BuildOffsetVectors();

	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = 256;
	texDesc.Height = 256;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	m_RandomVectorMap = CommonData::_ResourceManager()->BuildResource(&texDesc, FlagPack);
	// ·£´ý ¹éÅÍ ¸Ê ¾÷·Îµå ¹öÆÛ ºôµå
	const UINT num2DSubresources = texDesc.DepthOrArraySize * texDesc.MipLevels;
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_RandomVectorMap, 0, num2DSubresources);
	FlagPack.Property = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	m_RandomVectorMapUploadBuffer = CommonData::_ResourceManager()->BuildResource(
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize), FlagPack);

	XMCOLOR initData[256 * 256];
	for (int i = 0; i < 256; ++i)
	{
		for (int j = 0; j < 256; ++j)
		{
			// Random vector in [0,1].  We will decompress in shader to [-1,1].
			XMFLOAT3 v(MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF());

			initData[i * 256 + j] = XMCOLOR(v.x, v.y, v.z, 0.0f);
		}
	}
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = 256 * sizeof(XMCOLOR);
	subResourceData.SlicePitch = subResourceData.RowPitch * 256;

	CommonData::_ResourceManager()->ResourceStateTransition(CommandList, m_RandomVectorMap,
		D3D12_RESOURCE_STATE_COPY_DEST);
	UpdateSubresources(CommandList, m_RandomVectorMap, m_RandomVectorMapUploadBuffer,
		0, 0, num2DSubresources, &subResourceData);
	CommonData::_ResourceManager()->ResourceStateTransition(CommandList, m_RandomVectorMap,
		D3D12_RESOURCE_STATE_GENERIC_READ);
}
void SSAO::ReBuildResource()
{
	m_NormalDesc.Width = m_Width;
	m_NormalDesc.Height = m_Height;
	float normalClearColor[] = { 0.0f,0.0f,1.0f,0.0f };
	CD3DX12_CLEAR_VALUE optClear(DXGI_FORMAT_R16G16B16A16_FLOAT, normalClearColor);

	ResourceFlagPack Flag;
	Flag.State = D3D12_RESOURCE_STATE_GENERIC_READ;
	m_NormalMap = CommonData::_ResourceManager()->ReBuildResource(m_NormalMap, &m_NormalDesc, Flag, &optClear);


	m_SSAODesc.Width = m_Width / 2;
	m_SSAODesc.Height = m_Height / 2;
	float SSAOClearColor[] = { 1.0f,1.0f,1.0f,1.0f };
	optClear = CD3DX12_CLEAR_VALUE(SSAOMapFormat, SSAOClearColor);
	m_SSAOMap_0 = CommonData::_ResourceManager()->ReBuildResource(m_SSAOMap_0, &m_SSAODesc, Flag, &optClear);
	m_SSAOMap_1 = CommonData::_ResourceManager()->ReBuildResource(m_SSAOMap_1, &m_SSAODesc, Flag, &optClear);
}
void SSAO::BuildDescriptor()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = NormalMapFormat;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	m_NormalMapPack = CommonData::_ResourceManager()->AddSrv("SSAO_Normal_Srv", m_NormalMap, &srvDesc);
	// ±íÀÌ ¹öÆÛ
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	m_DepthMapPack = CommonData::_ResourceManager()->AddSrv("SSAO_Depth_Srv", CommonData::_Core()->DepthStencilBuffer(), &srvDesc);


	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_RandomVectorMapPack = CommonData::_ResourceManager()->AddSrv("SSAO_RandomVectorMap_Srv", m_RandomVectorMap, &srvDesc);


	srvDesc.Format = SSAOMapFormat;
	m_SSAOMapPack_0 = CommonData::_ResourceManager()->AddSrv("SSAO_SSAOMap_Srv_0", m_SSAOMap_0, &srvDesc);
	m_SSAOMapPack_1 = CommonData::_ResourceManager()->AddSrv("SSAO_SSAOMap_Srv_1", m_SSAOMap_1, &srvDesc);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = NormalMapFormat;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	m_NormalMapRtvPack = CommonData::_ResourceManager()->AddRtv("SSAO_Normal_Rtv", m_NormalMap,
		&rtvDesc);

	rtvDesc.Format = SSAOMapFormat;
	m_SSAORtvPack_0 = CommonData::_ResourceManager()->AddRtv("SSAO_SSAOMap_Rtv_0", m_SSAOMap_0,
		&rtvDesc);
	m_SSAORtvPack_1 = CommonData::_ResourceManager()->AddRtv("SSAO_SSAOMap_Rtv_1", m_SSAOMap_1,
		&rtvDesc);
}
void SSAO::ReBuildDescrptor()
{
	m_NormalMapPack = CommonData::_ResourceManager()->SetSrv(m_NormalMapPack->PackName, m_NormalMap);
	m_DepthMapPack = CommonData::_ResourceManager()->SetSrv(m_DepthMapPack->PackName, CommonData::_Core()->DepthStencilBuffer());
	m_SSAOMapPack_0 = CommonData::_ResourceManager()->SetSrv(m_SSAOMapPack_0->PackName, m_SSAOMap_0);
	m_SSAOMapPack_1 = CommonData::_ResourceManager()->SetSrv(m_SSAOMapPack_1->PackName, m_SSAOMap_1);
	// ·»´õ Å¸°Ù
	m_NormalMapRtvPack = CommonData::_ResourceManager()->SetRtv(m_NormalMapRtvPack->PackName, m_NormalMap);
	m_SSAORtvPack_0 = CommonData::_ResourceManager()->SetRtv(m_SSAORtvPack_0->PackName, m_SSAOMap_0);
	m_SSAORtvPack_1 = CommonData::_ResourceManager()->SetRtv(m_SSAORtvPack_1->PackName, m_SSAOMap_1);
}
void SSAO::DrawNormalDepthMap(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList)
{
	CommandList->RSSetViewports(1, &m_Viewport);
	CommandList->RSSetScissorRects(1, &m_ScissorRect);
	// ³ë¸Ö ¸Ê ±×¸®±â
	CommonData::_ResourceManager()->ResourceStateTransition(
		CommandList, m_NormalMap, D3D12_RESOURCE_STATE_RENDER_TARGET);
	float clearValue[] = { 0.0f,0.0f,1.0f,0.0f };
	CommandList->ClearRenderTargetView(m_NormalMapRtvPack->Handle, clearValue, 0, nullptr);
	CommandList->ClearDepthStencilView(CommonData::_Core()->DepthStencilView(), 
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	CommandList->OMSetRenderTargets(1, &m_NormalMapRtvPack->Handle, true, &CommonData::_Core()->DepthStencilView());
	// ¸ÞÀÎ ÆÐ¾² µî·Ï
	CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerPass,
		CommonData::_Scene()->MainPassHandle());


	CommandList->SetPipelineState(m_NormalMapPSO);
	for (auto& obj : CommonData::_Scene()->GetArray(EPSOMode::DEFAULT))
	{
		obj->Draw(CurrFrameResource, CommandList, EObjRenderMode::NonePSO);
	}
	CommonData::_ResourceManager()->ResourceStateTransition(CommandList, m_NormalMap,
		D3D12_RESOURCE_STATE_GENERIC_READ);
}
void SSAO::DrawSSAO(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList)
{
	CommandList->SetGraphicsRootSignature(m_SSAORootSig->GetRootSignature());
	CommandList->RSSetViewports(1, &m_SSAOViewport);
	CommandList->RSSetScissorRects(1, &m_SSAOScissorRect);

	// We compute the initial SSAO to AmbientMap0.

	// Change to RENDER_TARGET.
	CommonData::_ResourceManager()->ResourceStateTransition(CommandList, m_SSAOMap_0,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	float clearValue[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	CommandList->ClearRenderTargetView(m_SSAORtvPack_0->Handle, clearValue, 0, nullptr);

	// Specify the buffers we are going to render to.
	CommandList->OMSetRenderTargets(1, &m_SSAORtvPack_0->Handle, true, nullptr);

	// Bind the constant buffer for this pass.
	auto ssaoCBAddress = m_SSAOCB->Resource()->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView((UINT)ESSAOShaderSlot::SSAOData, ssaoCBAddress);
	CommandList->SetGraphicsRoot32BitConstant((UINT)ESSAOShaderSlot::Empty_Consant, 0, 0);

	// Bind the normal and depth maps.
	CommandList->SetGraphicsRootDescriptorTable((UINT)ESSAOShaderSlot::NormalMap, GetNormalMapGPUHandle());
	CommandList->SetGraphicsRootDescriptorTable((UINT)ESSAOShaderSlot::DepthMap, GetDepthMapGPUHandle());
	// Bind the random vector map.
	CommandList->SetGraphicsRootDescriptorTable((UINT)ESSAOShaderSlot::RandomVecMap, GetRandomVectorGPUHandle());

	CommandList->SetPipelineState(m_SSAOPSO);

	// Draw fullscreen quad.
	CommandList->IASetVertexBuffers(0, 0, nullptr);
	CommandList->IASetIndexBuffer(nullptr);
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->DrawInstanced(6, 1, 0, 0);
	CommonData::_ResourceManager()->ResourceStateTransition(CommandList, m_SSAOMap_0, D3D12_RESOURCE_STATE_GENERIC_READ);

	BlurSSAOMap(CommandList, CurrFrameResource, 3);
}
void SSAO::BlurSSAOMap(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, int blurCount)
{
	// pso ºí·¯ ¼ÂÆÃ
	CommandList->SetPipelineState(m_SSAOBlurPSO);
	auto ssaoCBAddress = m_SSAOCB->Resource()->GetGPUVirtualAddress();
	CommandList->SetGraphicsRootConstantBufferView((UINT)ESSAOShaderSlot::SSAOData, ssaoCBAddress);
	for (int i = 0; i < blurCount; ++i)
	{
		BlurSSAOMap(CommandList, true);
		BlurSSAOMap(CommandList, false);
	}
}
void SSAO::BlurSSAOMap(ID3D12GraphicsCommandList* CommandList, bool horzBlur)
{
	ID3D12Resource* output = nullptr;
	CD3DX12_GPU_DESCRIPTOR_HANDLE inputSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE outputRtv;

	if (horzBlur == true)
	{
		output = m_SSAOMap_1;
		inputSrv = GetSSAOMapGPUHandle_0();
		outputRtv = m_SSAORtvPack_1->Handle;
		CommandList->SetGraphicsRoot32BitConstant((UINT)ESSAOShaderSlot::Empty_Consant, 1, 0);
	}
	else
	{
		output = m_SSAOMap_0;
		inputSrv = GetSSAOMapGPUHandle_1();
		outputRtv = m_SSAORtvPack_0->Handle;
		CommandList->SetGraphicsRoot32BitConstant((UINT)ESSAOShaderSlot::Empty_Consant, 0, 0);
	}
	CommonData::_ResourceManager()->ResourceStateTransition(CommandList, output,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	float clearValue[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	CommandList->ClearRenderTargetView(outputRtv, clearValue, 0, nullptr);
	CommandList->OMSetRenderTargets(1, &outputRtv, true, nullptr);

	CommandList->SetGraphicsRootDescriptorTable((UINT)ESSAOShaderSlot::NormalMap, GetNormalMapGPUHandle());
	CommandList->SetGraphicsRootDescriptorTable((UINT)ESSAOShaderSlot::DepthMap, GetDepthMapGPUHandle());
	// Bind the random vector map.
	CommandList->SetGraphicsRootDescriptorTable((UINT)ESSAOShaderSlot::RandomVecMap, inputSrv);

	CommandList->IASetVertexBuffers(0, 0, nullptr);
	CommandList->IASetIndexBuffer(nullptr);
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->DrawInstanced(6, 1, 0, 0);

	CommonData::_ResourceManager()->ResourceStateTransition(CommandList, output,
		D3D12_RESOURCE_STATE_GENERIC_READ);
}
void SSAO::BuildOffsetVectors()
{
	// 8 cube corners
	m_Offsets[0] = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
	m_Offsets[1] = XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);

	m_Offsets[2] = XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
	m_Offsets[3] = XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);

	m_Offsets[4] = XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
	m_Offsets[5] = XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);

	m_Offsets[6] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
	m_Offsets[7] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

	// 6 centers of cube faces
	m_Offsets[8] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	m_Offsets[9] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f);

	m_Offsets[10] = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	m_Offsets[11] = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f);

	m_Offsets[12] = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	m_Offsets[13] = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f);

	for (int i = 0; i < 14; ++i)
	{
		// Create random lengths in [0.25, 1.0].
		float s = MathHelper::RandF(0.25f, 1.0f);

		XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&m_Offsets[i]));

		XMStoreFloat4(&m_Offsets[i], v);
	}
}
std::vector<float> SSAO::CalcGaussWeights(float sigma)
{
	float twoSigma2 = 2.0f*sigma*sigma;

	// Estimate the blur radius based on sigma since sigma controls the "width" of the bell curve.
	// For example, for sigma = 3, the width of the bell curve is 
	int blurRadius = (int)ceil(2.0f * sigma);

	assert(blurRadius <= MaxBlurRadius);

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