#include"CubeMap.h"
#include"DxCore/DxDevice.h"
#include"Shader/CommonShaderRootSignature.h"
#include"Shader/Shader.h"
#include"Data/Scene.h"
#include"Data/CommonData.h"
#include"Data/SceneData.h"
#include"Data/JGRCObject.h"
#include"Data/JGMaterial.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;

UINT CubeMap::m_Width  = 512;
UINT CubeMap::m_Height = 512;
CubeMap::CubeMap(UINT Width, UINT Height)
{
	m_Width = Width;
	m_Height = Height;
}
void CubeMap::BuildCubeMap(const string& objName, ID3D12GraphicsCommandList* CommandList)
{
	m_ObjName = objName;
	m_ViewPort = { 0.0f,0.0f,(float)m_Width, (float)m_Height,0.0f,1.0f };
	m_ScissorRect = { 0,0,(int)m_Width, (int)m_Height };
	m_CubeScene = make_unique<SceneData>();
	m_CubeScene->BuildSceneData(m_Width, m_Height);
	BuildResource(CommandList);
	BuildDescriptors();
	// 패쓰 데이터 가져오기
	for (int i = 0; i < 6; ++i)
	{
		m_CubeMapPass[i] = CommonData::_Scene()->AddPassData();
	}
}
void CubeMap::Update(const GameTimer& gt,FrameResource* CurrentResource)
{
	for (int i = 0; i < 6; ++i)
	{
		m_CubeMapPass[i]->Data = CommonData::_Scene()->MainPassData();

		XMMATRIX view = m_CubeCamera[i].GetView();
		XMMATRIX proj = m_CubeCamera[i].GetProj();

		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
		XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
		XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

		XMStoreFloat4x4(&m_CubeMapPass[i]->Data.View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&m_CubeMapPass[i]->Data.InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&m_CubeMapPass[i]->Data.Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&m_CubeMapPass[i]->Data.InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&m_CubeMapPass[i]->Data.ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&m_CubeMapPass[i]->Data.InvViewProj, XMMatrixTranspose(invViewProj));
		m_CubeMapPass[i]->Data.EyePosW = m_CubeCamera[i].GetPosition3f();
		m_CubeMapPass[i]->Data.RenderTargetSize = XMFLOAT2((float)m_Width, (float)m_Height);
		m_CubeMapPass[i]->Data.InvRenderTargetSize = XMFLOAT2(1.0f / m_Width, 1.0f / m_Height);

		m_CubeMapPass[i]->Data.WorldPosSceneIndex = m_CubeScene->GetWorldPosIndex();
		m_CubeMapPass[i]->Data.AlbedoSceneIndex = m_CubeScene->GetAlbedoIndex();
		m_CubeMapPass[i]->Data.NormalSceneIndex = m_CubeScene->GetNormalIndex();
		m_CubeMapPass[i]->Data.DepthSceneIndex = m_CubeScene->GetDepthIndex();
		m_CubeMapPass[i]->Data.MatSceneIndex = m_CubeScene->GetMatIndex();
		if (CommonData::_Scene()->GetMainSkyBox())
			m_CubeMapPass[i]->Data.SkyBoxIndex = CommonData::_ResourceManager()->GetCubeTextureShaderIndex(
				CommonData::_Scene()->GetMainSkyBox()->GetMaterial()->GetTexturePath(ETextureSlot::Diffuse));

		auto currPassCB = CurrentResource->PassCB.get();
		currPassCB->CopyData(m_CubeMapPass[i]->PassCBIndex, m_CubeMapPass[i]->Data);
	}
}
void CubeMap::Excute(FrameResource* CurrentResource,ID3D12GraphicsCommandList* CommandList, bool isDynamic)
{
	CommandList->RSSetViewports(1, &m_ViewPort);
	CommandList->RSSetScissorRects(1, &m_ScissorRect);
	CommonData::_ResourceManager()->ResourceStateTransition(CommandList, m_CubeMap, D3D12_RESOURCE_STATE_RENDER_TARGET);
	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(cbPassConstant));
	for (int i = 0; i < 6; ++i)
	{
		auto passCB = CurrentResource->PassCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + m_CubeMapPass[i]->PassCBIndex * passCBByteSize;

		CommandList->SetGraphicsRootConstantBufferView((int)ECommonShaderSlot::cbPerPass, passCBAddress);


		m_CubeScene->SceneDataExtract(CurrentResource, CommandList);

		CommandList->ClearRenderTargetView(RtvPack[i]->Handle, Colors::LightSteelBlue, 0, nullptr);
		CommandList->ClearDepthStencilView(DsvPack->Handle,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0F, 0, 0, nullptr);

		CommandList->OMSetRenderTargets(1, &RtvPack[i]->Handle, true, &DsvPack->Handle);

		CommandList->SetPipelineState(CommonData::_Scene()->GetScenePSO());
		CommandList->IASetVertexBuffers(0, 0, nullptr);
		CommandList->IASetIndexBuffer(nullptr);
		CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		CommandList->DrawInstanced(6, 1, 0, 0);


	}
	CommonData::_ResourceManager()->ResourceStateTransition(CommandList, m_CubeMap, D3D12_RESOURCE_STATE_GENERIC_READ);
}
UINT CubeMap::GetCubeMapIndex()
{
	return CommonData::_ResourceManager()->GetCubeMapShaderIndex(m_ObjName + "Srv");
}
void CubeMap::BuildDescriptors()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	CommonData::_ResourceManager()->AddSrv(m_ObjName + "Srv", m_CubeMap, &srvDesc);
	for (int i = 0; i < 6; ++i)
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Format = m_Format;
		rtvDesc.Texture2DArray.MipSlice = 0;
		rtvDesc.Texture2DArray.PlaneSlice = 0;

		rtvDesc.Texture2DArray.FirstArraySlice = i;
		rtvDesc.Texture2DArray.ArraySize = 1;

		RtvPack[i] = CommonData::_ResourceManager()->AddRtv(m_ObjName + "Rtv" + to_string(i), m_CubeMap, &rtvDesc);
	}
}
void CubeMap::BuildResource(ID3D12GraphicsCommandList* CommandList)
{
	D3D12_RESOURCE_DESC Desc;
	ZeroMemory(&Desc, sizeof(D3D12_RESOURCE_DESC));
	Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	Desc.Alignment = 0;
	Desc.Width = m_Width;
	Desc.Height = m_Height;
	Desc.DepthOrArraySize = 6;
	Desc.MipLevels = 1;
	Desc.Format = m_Format;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	ResourceFlagPack RFlagPack;
	RFlagPack.State = D3D12_RESOURCE_STATE_GENERIC_READ;

	float clearColor[] = { 1.0f,1.0f,1.0f,1.0f };
	CD3DX12_CLEAR_VALUE value(m_Format, Colors::LightSteelBlue);

	m_CubeMap = CommonData::_ResourceManager()->BuildResource(&Desc, RFlagPack, &value);


	ZeroMemory(&Desc, sizeof(D3D12_RESOURCE_DESC));
	Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	Desc.Alignment = 0;
	Desc.Width = m_Width;
	Desc.Height = m_Height;
	Desc.DepthOrArraySize = 1;
	Desc.MipLevels = 1;
	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	m_DepthStencilBuffer = CommonData::_ResourceManager()->BuildResource(&Desc, ResourceFlagPack(), &optClear);

	DsvPack = CommonData::_ResourceManager()->AddDsv(m_ObjName + "Dsv", m_DepthStencilBuffer, nullptr);

	CommonData::_ResourceManager()->ResourceStateTransition(CommandList, m_DepthStencilBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE);
}
void CubeMap::BuildCamera(float x, float y, float z)
{
	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

	// Look along each coordinate axis.
	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f, y, z), // +X
		XMFLOAT3(x - 1.0f, y, z), // -X
		XMFLOAT3(x, y + 1.0f, z), // +Y
		XMFLOAT3(x, y - 1.0f, z), // -Y
		XMFLOAT3(x, y, z + 1.0f), // +Z
		XMFLOAT3(x, y, z - 1.0f)  // -Z
	};

	// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
	// are looking down +Y or -Y, so we need a different "up" vector.
	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
		XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
	};
	for (int i = 0; i < 6; ++i)
	{
		m_CubeCamera[i].LookAt(center, targets[i], ups[i]);
		m_CubeCamera[i].SetLens(0.5f*XM_PI, 1.0f, 0.1f, 1000.0f);
		m_CubeCamera[i].UpdateViewMatrix();
	}
}

