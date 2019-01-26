#include"SceneData.h"
#include"CommonData.h"
#include"Scene.h"
using namespace std;
using namespace JGRC;
using namespace DirectX;

UINT SceneData::Count = 0;

SceneData::SceneData()
{
	Count++;
	m_Name += to_string(Count);
}
void SceneData::BuildSceneData(UINT width, UINT height)
{
	m_Width  = width;
	m_Height = height;
	m_ViewPort    = { 0.0f,0.0f,(float)m_Width, (float)m_Height,0.0f,1.0f };
	m_ScissorRect = { 0,0,(int)m_Width, (int)m_Height };
	BuildResource();
	RegisterHeap();
}
void SceneData::ReSize(UINT width, UINT height)
{
	if (m_Width != width || m_Height != height)
	{
		m_Width = width;
		m_Height = height;
		m_ViewPort = { 0.0f,0.0f,(float)m_Width, (float)m_Height,0.0f,1.0f };
		m_ScissorRect = { 0,0,(int)m_Width, (int)m_Height };
		ReBuildResource();
		ReRegisterHeap();
	}
}
void SceneData::SceneDataExtract(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList)
{
	CommandList->RSSetViewports(1, &m_ViewPort);
	CommandList->RSSetScissorRects(1, &m_ScissorRect);

	float clearColor[] = { 0.0f,0.0f,0.0f,0.0f };
	auto RsManager = CommonData::_ResourceManager();
	RsManager->ResourceStateTransition(CommandList, m_WorldPosPack->Resource,	D3D12_RESOURCE_STATE_RENDER_TARGET);
	RsManager->ResourceStateTransition(CommandList, m_AlbedoPack->Resource, D3D12_RESOURCE_STATE_RENDER_TARGET);
	RsManager->ResourceStateTransition(CommandList, m_NormalPack->Resource, D3D12_RESOURCE_STATE_RENDER_TARGET);
	RsManager->ResourceStateTransition(CommandList, m_MatPack->Resource, D3D12_RESOURCE_STATE_RENDER_TARGET);
	RsManager->ResourceStateTransition(CommandList, m_DepthPack->Resource, D3D12_RESOURCE_STATE_DEPTH_WRITE);


	CommandList->ClearRenderTargetView(m_WorldPosRtv->Handle, clearColor, 0, nullptr);
	CommandList->ClearRenderTargetView(m_AlbedoRtv->Handle, clearColor, 0, nullptr);
	CommandList->ClearRenderTargetView(m_NormalRtv->Handle, clearColor, 0, nullptr);
	CommandList->ClearRenderTargetView(m_MatRtv->Handle, clearColor, 0, nullptr);
	CommandList->ClearDepthStencilView(m_DepthDsv->Handle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0F, 0, 0, nullptr);


	D3D12_CPU_DESCRIPTOR_HANDLE RtvHandle[] = { m_WorldPosRtv->Handle,m_AlbedoRtv->Handle ,m_NormalRtv->Handle, m_MatRtv->Handle };
	CommandList->OMSetRenderTargets(4, RtvHandle, true, &m_DepthDsv->Handle);

	CommonData::_Scene()->SceneObjectDraw(CommandList, CurrFrameResource, EObjRenderMode::Default);
	if (CommonData::_Scene()->GetMainSkyBox())
	{
		CommonData::_Scene()->GetMainSkyBox()->Draw(CurrFrameResource, CommandList);
	}
	RsManager->ResourceStateTransition(CommandList, m_WorldPosPack->Resource, D3D12_RESOURCE_STATE_GENERIC_READ);
	RsManager->ResourceStateTransition(CommandList, m_AlbedoPack->Resource, D3D12_RESOURCE_STATE_GENERIC_READ);
	RsManager->ResourceStateTransition(CommandList, m_NormalPack->Resource, D3D12_RESOURCE_STATE_GENERIC_READ);
	RsManager->ResourceStateTransition(CommandList, m_MatPack->Resource, D3D12_RESOURCE_STATE_GENERIC_READ);
	RsManager->ResourceStateTransition(CommandList, m_DepthPack->Resource, D3D12_RESOURCE_STATE_GENERIC_READ);
}

UINT SceneData::GetWorldPosIndex() const
{
	return CommonData::_ResourceManager()->GetSrvUavIndex(m_WorldPosPack->PackName);
}
UINT SceneData::GetAlbedoIndex()   const
{
	return CommonData::_ResourceManager()->GetSrvUavIndex(m_AlbedoPack->PackName);
}
UINT SceneData::GetNormalIndex()   const
{
	return CommonData::_ResourceManager()->GetSrvUavIndex(m_NormalPack->PackName);
}
UINT SceneData::GetMatIndex()      const
{
	return CommonData::_ResourceManager()->GetSrvUavIndex(m_MatPack->PackName);
}
UINT SceneData::GetDepthIndex()    const
{
	return CommonData::_ResourceManager()->GetSrvUavIndex(m_DepthPack->PackName);
}
void SceneData::BuildResource()
{
	D3D12_RESOURCE_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D12_RESOURCE_DESC));
	Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	Desc.Width  = m_Width;
	Desc.Height = m_Height;
	Desc.DepthOrArraySize = 1;
	Desc.MipLevels = 1;
	Desc.SampleDesc.Count   = 1;
	Desc.SampleDesc.Quality = 0;

	m_WorldPosDesc = Desc;
	m_AlbedoDesc   = Desc;
	m_NormalDesc   = Desc;
	m_DepthDesc    = Desc;
	m_MatDesc      = Desc;


	// 세계 좌표 목록
	m_WorldPosDesc.Format = m_WorldPosFormat;
	m_WorldPosDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_WorldPosDesc.Flags  = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	// 물체 색 목록
	m_AlbedoDesc.Format = m_AlbedoFormat;
	m_AlbedoDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_AlbedoDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	// 노멀 목록
	m_NormalDesc.Format = m_NormalFormat;
	m_NormalDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_NormalDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	// 머터리얼 정보
	m_MatDesc.Format = m_MatFormat;
	m_MatDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_MatDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	// 깊이 목록
	m_DepthDesc.Format = m_DepthFormat;
	m_DepthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	m_DepthDesc.Flags  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
	m_ResourceFlag.State = D3D12_RESOURCE_STATE_GENERIC_READ;

	D3D12_CLEAR_VALUE ClearValue;
	for (int i = 0; i < 4; ++i)
	{
		ClearValue.Color[i] = 0.0f;
	}
	auto RsManager = CommonData::_ResourceManager();

	// 세계좌표 리소스 생성
	ClearValue.Format = m_WorldPosFormat;
	m_WorldPosResource = RsManager->BuildResource(&m_WorldPosDesc, m_ResourceFlag, &ClearValue);
	// albedo 리소스 생성
	ClearValue.Format = m_AlbedoFormat;
	m_AlbedoResource = RsManager->BuildResource(&m_AlbedoDesc, m_ResourceFlag, &ClearValue);
	// 세계 노멀 리소스 
	ClearValue.Format = m_NormalFormat;
	m_NormalResource = RsManager->BuildResource(&m_NormalDesc, m_ResourceFlag, &ClearValue);
	// 머터리얼 정보
	ClearValue.Format = m_MatFormat;
	m_MatResource = RsManager->BuildResource(&m_MatDesc, m_ResourceFlag, &ClearValue);

	// 깊이 리소스
	D3D12_CLEAR_VALUE DepthClearValue;
	DepthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthClearValue.DepthStencil.Depth   = 1.0f;
	DepthClearValue.DepthStencil.Stencil = 0;
	m_DepthResource = RsManager->BuildResource(&m_DepthDesc, m_ResourceFlag, &DepthClearValue);
}
void SceneData::ReBuildResource()
{
	auto RsManager = CommonData::_ResourceManager();



	D3D12_CLEAR_VALUE ClearValue;
	for (int i = 0; i < 4; ++i)
	{
		ClearValue.Color[i] = 0.0f;
	}

	// 월드 좌표
	m_WorldPosDesc.Width = m_Width;
	m_WorldPosDesc.Height = m_Height;
	ClearValue.Format = m_WorldPosFormat;
	m_WorldPosResource = RsManager->ReBuildResource(m_WorldPosResource, &m_WorldPosDesc, m_ResourceFlag, &ClearValue);

	// Albedo
	m_AlbedoDesc.Width = m_Width;
	m_AlbedoDesc.Height = m_Height;
	ClearValue.Format = m_AlbedoFormat;
	m_AlbedoResource = RsManager->ReBuildResource(m_AlbedoResource, &m_AlbedoDesc, m_ResourceFlag, &ClearValue);



	// 노멀 법선
	m_NormalDesc.Width = m_Width;
	m_NormalDesc.Height = m_Height;
	ClearValue.Format = m_NormalFormat;
	m_NormalResource = RsManager->ReBuildResource(m_NormalResource, &m_NormalDesc, m_ResourceFlag, &ClearValue);

	// 머터리얼 정보
	m_MatDesc.Width = m_Width;
	m_MatDesc.Height = m_Height;
	ClearValue.Format = m_MatFormat;
	m_MatResource = RsManager->ReBuildResource(m_MatResource, &m_MatDesc, m_ResourceFlag, &ClearValue);
	// 깊이
	D3D12_CLEAR_VALUE DepthClearValue;
	DepthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthClearValue.DepthStencil.Depth = 1.0f;
	DepthClearValue.DepthStencil.Stencil = 0;
	m_DepthDesc.Width = m_Width;
	m_DepthDesc.Height = m_Height;
	m_DepthResource = RsManager->ReBuildResource(m_DepthResource, &m_DepthDesc, m_ResourceFlag, &DepthClearValue);
}
void SceneData::RegisterHeap()
{
	auto RsManager = CommonData::_ResourceManager();
	
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	//
	srvDesc.Format = m_WorldPosFormat;
	m_WorldPosPack = RsManager->AddSrv(m_Name + "_WorldPos_Srv", m_WorldPosResource, &srvDesc);


	//
	srvDesc.Format = m_AlbedoFormat;
	m_AlbedoPack = RsManager->AddSrv(m_Name + "_Albedo_Srv", m_AlbedoResource, &srvDesc);


	//
	srvDesc.Format = m_NormalFormat;
	m_NormalPack = RsManager->AddSrv(m_Name + "_Normal_Srv", m_NormalResource, &srvDesc);


	//
	srvDesc.Format = m_MatFormat;
	m_MatPack = RsManager->AddSrv(m_Name + "_Mat_Srv", m_MatResource, &srvDesc);

	//
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	m_DepthPack = RsManager->AddSrv(m_Name + "_Depth_Srv", m_DepthResource, &srvDesc);


	//// 렌더 타겟 설정
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice  = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;
	

	//
	rtvDesc.Format = m_WorldPosFormat;
	m_WorldPosRtv = RsManager->AddRtv(m_Name + "_WorldPos_Rtv", m_WorldPosResource, &rtvDesc);


	// 
	rtvDesc.Format = m_AlbedoFormat;
	m_AlbedoRtv = RsManager->AddRtv(m_Name + "_Albedo_Rtv", m_AlbedoResource, &rtvDesc);
	

	//
	rtvDesc.Format = m_NormalFormat;
	m_NormalRtv = RsManager->AddRtv(m_Name + "_Normal_Rtv", m_NormalResource, &rtvDesc);

	//
	rtvDesc.Format = m_MatFormat;
	m_MatRtv = RsManager->AddRtv(m_Name + "_Mat_Rtv", m_MatResource, &rtvDesc);


	//
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;

	m_DepthDsv = RsManager->AddDsv(m_Name + "_Depth_Dsv", m_DepthResource, &dsvDesc);
}
void SceneData::ReRegisterHeap()
{
	auto RsManager = CommonData::_ResourceManager();
	m_WorldPosPack = RsManager->SetSrv(m_WorldPosPack->PackName, m_WorldPosResource);
	m_AlbedoPack = RsManager->SetSrv(m_AlbedoPack->PackName, m_AlbedoResource);
	m_NormalPack = RsManager->SetSrv(m_NormalPack->PackName, m_NormalResource);
	m_MatPack = RsManager->SetSrv(m_MatPack->PackName, m_MatResource);
	m_DepthPack = RsManager->SetSrv(m_DepthPack->PackName, m_DepthResource);



	m_WorldPosRtv = RsManager->SetRtv(m_WorldPosRtv->PackName, m_WorldPosResource);
	m_AlbedoRtv = RsManager->SetRtv(m_AlbedoRtv->PackName, m_AlbedoResource);
	m_NormalRtv = RsManager->SetRtv(m_NormalRtv->PackName, m_NormalResource);
	m_MatRtv = RsManager->SetRtv(m_MatRtv->PackName, m_MatResource);
	m_DepthDsv = RsManager->SetDsv(m_DepthDsv->PackName, m_DepthResource);
}