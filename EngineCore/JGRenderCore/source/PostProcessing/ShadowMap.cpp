#include"ShadowMap.h"
#include"Data/Scene.h"
#include"Data/JGLight.h"
#include"Data/CommonData.h"
using namespace std;
using namespace JGRC;
using namespace DirectX;
UINT ShadowMap::m_Width  = 1024;
UINT ShadowMap::m_Height = 1024;
D3D12_VIEWPORT ShadowMap::m_Viewport;
D3D12_RECT     ShadowMap::m_ScissorRect;
ShadowMap::ShadowMap(UINT width, UINT height)
{
	m_Width  = width;
	m_Height = height;
	m_Viewport = { 0.0f,0.0f,(float)width, (float)height,0.0f,1.0f };
	m_ScissorRect = { 0,0,(int)width, (int)height };
}

void ShadowMap::Draw(FrameResource* CurrentFrameResource, ID3D12GraphicsCommandList* CommandList)
{
	CommandList->RSSetViewports(1, &m_Viewport);
	CommandList->RSSetScissorRects(1, &m_ScissorRect);
	Default_Draw(CurrentFrameResource, CommandList);
}
void ShadowMap::UpdateShadowPass(FrameResource* CurrentFrameResource, JGLight* light)
{
	XMFLOAT4X4 view_f4x4, proj_f4x4;
	XMMATRIX View, Proj, viewProj;
	auto PassCB = CurrentFrameResource->PassCB.get();
	switch (m_Type)
	{
	case EShadowMapType::Spot:
	case EShadowMapType::Direction:
		view_f4x4 = light->GetViewMatrix();
		proj_f4x4 = light->GetProjMatrix();
		View = XMLoadFloat4x4(&view_f4x4);
		Proj = XMLoadFloat4x4(&proj_f4x4);
		viewProj = XMMatrixMultiply(View, Proj);
		XMStoreFloat4x4(&m_ShadowPass->Data.ViewProj, XMMatrixTranspose(viewProj));
		PassCB->CopyData(m_ShadowPass->PassCBIndex, m_ShadowPass->Data);
		break;
	case EShadowMapType::Point:	
		for (int i = 0; i < 6; ++i)
		{
			View = light->GetPointLightCamera(i).GetView();
			Proj = light->GetPointLightCamera(i).GetProj();
			viewProj = XMMatrixMultiply(View, Proj);
			XMStoreFloat4x4(&m_CubeShadowPass[i]->Data.ViewProj, XMMatrixTranspose(viewProj));
			PassCB->CopyData(m_CubeShadowPass[i]->PassCBIndex, m_CubeShadowPass[i]->Data);
		}
		break;
	}
}
void ShadowMap::BuildShadowMap(const string& name, EShadowMapType type)
{
	m_Type = type;
	BuildResource();
	BuildDescriptor(name);
	switch (type)
	{
	case EShadowMapType::Spot:
	case EShadowMapType::Direction:
		m_ShadowPass = CommonData::_Scene()->AddPassData();
		break;
	case EShadowMapType::Point:
		for (int i = 0; i < 6; ++i)
		{
			m_CubeShadowPass[i] = CommonData::_Scene()->AddPassData();
		}
		break;
	}
}
UINT ShadowMap::GetSrvIndex() const
{
	switch (m_Type)
	{
	case EShadowMapType::Direction:
	case EShadowMapType::Spot:
		return  CommonData::_ResourceManager()->GetSrvUavIndex(m_ShadowSrvPack->PackName);
	case EShadowMapType::Point:
		return  CommonData::_ResourceManager()->GetCubeMapShaderIndex(m_ShadowSrvPack->PackName);
	default:
		return -1;
	}
}
void ShadowMap::BuildResource()
{
	// 기본 목록 초기화
	ZeroMemory(&m_DefaultResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	m_DefaultResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_DefaultResourceDesc.Alignment = 0;
	m_DefaultResourceDesc.Width = m_Width;
	m_DefaultResourceDesc.Height = m_Height;
	m_DefaultResourceDesc.DepthOrArraySize = 1;
	m_DefaultResourceDesc.MipLevels = 1;
	m_DefaultResourceDesc.Format = m_Format;
	m_DefaultResourceDesc.SampleDesc.Count = 1;
	m_DefaultResourceDesc.SampleDesc.Quality = 0;
	m_DefaultResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_DefaultResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;


	ResourceFlagPack Pack;
	Pack.State = D3D12_RESOURCE_STATE_GENERIC_READ;

	switch (m_Type) // 포인트 라이트 셰도우 맵은 6 / 다이렉트, 스포트 라이트는 1 ( 깊이 배열 )
	{
	case EShadowMapType::Direction:
	case EShadowMapType::Spot:
		m_DefaultResourceDesc.DepthOrArraySize = 1;
		break;
	case EShadowMapType::Point:
		m_DefaultResourceDesc.DepthOrArraySize = 6;
		break;
	}

	m_ShadowMap = CommonData::_ResourceManager()->BuildResource(&m_DefaultResourceDesc, Pack, &optClear);
}
void ShadowMap::BuildDescriptor(const string& name)
{
	m_SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	m_SrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	switch (m_Type) 
	{
	case EShadowMapType::Direction:
	case EShadowMapType::Spot:
		// Srv
		m_SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		m_SrvDesc.Texture2D.MostDetailedMip = 0;
		m_SrvDesc.Texture2D.MipLevels = 1;
		m_SrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		m_SrvDesc.Texture2D.PlaneSlice = 0;
		m_ShadowSrvPack = CommonData::_ResourceManager()->AddSrv(name + "Srv", m_ShadowMap, &m_SrvDesc);
		// Dsv
		m_DsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		m_DsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		m_DsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_DsvDesc.Texture2D.MipSlice = 0;
		m_DepthStencilViewPack = CommonData::_ResourceManager()->AddDsv(name + "Dsv", m_ShadowMap, &m_DsvDesc);
		break;
	case EShadowMapType::Point:
		// Srv
		m_SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		m_SrvDesc.TextureCube.MostDetailedMip = 0;
		m_SrvDesc.TextureCube.MipLevels = 1;
		m_SrvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		m_ShadowSrvPack = CommonData::_ResourceManager()->AddSrv(name + "Srv", m_ShadowMap, &m_SrvDesc);
		// Dsv
		for (int i = 0; i < 6; ++i)
		{
			m_DsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			m_DsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			m_DsvDesc.Texture2DArray.MipSlice = 0;
			m_DsvDesc.Texture2DArray.ArraySize = 1;
			m_DsvDesc.Texture2DArray.FirstArraySlice = i;
			m_CubeDepthStencilViewPack[i] = CommonData::_ResourceManager()->AddDsv(name + "Dsv" + to_string(i), m_ShadowMap, &m_DsvDesc);
		}
		break;
	}
}
void ShadowMap::Default_Draw(FrameResource* CurrentFrameResource, ID3D12GraphicsCommandList* CommandList)
{
	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(cbPassConstant));
	auto PassCB = CurrentFrameResource->PassCB->Resource();
	D3D12_GPU_VIRTUAL_ADDRESS passCBAddress;

	CommonData::_ResourceManager()->ResourceStateTransition(
		CommandList,
		m_ShadowMap,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	switch (m_Type)
	{
	case EShadowMapType::Direction:
	case EShadowMapType::Spot:
		passCBAddress =
			PassCB->GetGPUVirtualAddress() + (m_ShadowPass->PassCBIndex * passCBByteSize);
		CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerPass,
			passCBAddress);
		// 깊이 버퍼 초기화
		CommandList->ClearDepthStencilView(m_DepthStencilViewPack->Handle,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// 렌더 타겟에 깊이 뷰만 등록
		CommandList->OMSetRenderTargets(0, nullptr, false, &m_DepthStencilViewPack->Handle);

		for (auto& obj : CommonData::_Scene()->GetArray())
		{
			obj->Draw(CurrentFrameResource, CommandList, EObjRenderMode::NonePSO);
		}
		break;
	case EShadowMapType::Point:
		for (int i = 0; i < 6; ++i)
		{
			passCBAddress =
				PassCB->GetGPUVirtualAddress() + (m_CubeShadowPass[i]->PassCBIndex * passCBByteSize);
			CommandList->SetGraphicsRootConstantBufferView((UINT)ECommonShaderSlot::cbPerPass,
				passCBAddress);

			CommandList->ClearDepthStencilView(m_CubeDepthStencilViewPack[i]->Handle,
				D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
			//
			CommandList->OMSetRenderTargets(0, nullptr, false, &m_CubeDepthStencilViewPack[i]->Handle);
			for (auto& obj : CommonData::_Scene()->GetArray())
			{
				obj->Draw(CurrentFrameResource, CommandList, EObjRenderMode::NonePSO);
			}
		}
		break;
	}
	CommonData::_ResourceManager()->ResourceStateTransition(
		CommandList,
		m_ShadowMap,
		D3D12_RESOURCE_STATE_GENERIC_READ);
}