#include"ResourceManager.h"
#include"Data/JGRCObject.h"
#include"Data/JGMaterial.h"
#include"Data/Mesh/JGBaseMesh.h"
#include"Data/Mesh/JGSkeletalMesh.h"
#include"Data/Mesh/JGStaticMesh.h"
#include"DxCore/DxCore.h"
#include"Shader/CommonShaderRootSignature.h"
using namespace JGRC;
using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

void ResourceManager::Init(DxCore* core)
{
	m_Core = core;
	m_Device = core->Device();
}
ID3D12Resource* ResourceManager::BuildResource(D3D12_RESOURCE_DESC* desc, const ResourceFlagPack& Pack,
	D3D12_CLEAR_VALUE* ClearValue)
{
	ComPtr<ID3D12Resource> resource;

	m_Device->CreateCommittedResource(
		&Pack.Property,
		Pack.Flags,
		desc,
		Pack.State,
		ClearValue,
		IID_PPV_ARGS(resource.GetAddressOf()));
	ID3D12Resource* result = resource.Get();
	m_ResourceStates[result] = Pack.State;

	m_Resources.push_back(move(resource));
	return result;
}
ID3D12Resource* ResourceManager::BuildResource(IDXGISwapChain* swapChain, UINT idx)
{
	ComPtr<ID3D12Resource> resource;
	ThrowIfFailed(swapChain->GetBuffer(
		idx,
		IID_PPV_ARGS(resource.GetAddressOf())));
	ID3D12Resource* result = resource.Get();
	m_ResourceStates[result] = D3D12_RESOURCE_STATE_PRESENT;
	m_Resources.push_back(move(resource));
	return result;
}
ID3D12Resource* ResourceManager::ReBuildResource(ID3D12Resource* resource, D3D12_RESOURCE_DESC* desc, const ResourceFlagPack& Pack,
	D3D12_CLEAR_VALUE* ClearValue)
{
	auto iter = find_if(m_Resources.begin(), m_Resources.end(),
		[resource](ComPtr<ID3D12Resource> r) -> bool
	{
		if (r.Get() == resource)
			return true;
		return false;
	});
	if (iter == m_Resources.end())
		return nullptr;
	// 리소스 상태 삭제
	m_ResourceStates.erase((*iter).Get());
	(*iter).Reset();
	m_Device->CreateCommittedResource(
		&Pack.Property,
		Pack.Flags,
		desc,
		Pack.State,
		ClearValue,
		IID_PPV_ARGS((*iter).GetAddressOf()));
	ID3D12Resource* result = (*iter).Get();
	m_ResourceStates[result] = Pack.State;
	return result;
}
TexturePack* ResourceManager::AddTexture(ID3D12GraphicsCommandList* CommandList, const std::wstring& path, ETextureType type)
{
	if (m_Textures.end() != m_Textures.find(path))
	{
		// 예외 처리
		return nullptr;
	}
	// 텍스쳐 생성
	auto tex = make_unique<Texture>();
	tex->Name.assign(path.begin(), path.end());
	tex->Filename = path;
	ThrowIfFailed(CreateDDSTextureFromFile12(m_Device, CommandList,
		tex->Filename.c_str(), tex->Resource, tex->UploadHeap));

	// 생성

	switch (type)
	{
	case ETextureType::Default:
		m_Textures[path] = TexturePack{ path, move(tex), type };
		m_TextureOffsets[path]     = m_SrvHeapOffset;
		m_SrvHeapOffset++;
		return &m_Textures[path];
	case ETextureType::Cube:
		m_CubeTextures[path] = TexturePack{ path, move(tex), type };
		m_CubeTextureOffsets[path] = m_CubeHeapOffset;
		m_CubeHeapOffset++;
		return &m_CubeTextures[path];
	default:
		return nullptr;
	}
	
}
SrvResourcePack* ResourceManager::AddSrv(const string& name, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* Desc)
{
	if (m_SrvUavOffsets.end() != m_SrvUavOffsets.find(name))
	{
		// 예외 처리
		return nullptr;
	}
	// 리소스 저장


	switch (Desc->ViewDimension)
	{
	case D3D12_SRV_DIMENSION_TEXTURECUBE:
		m_CubeSrvResources[name] = SrvResourcePack{ name, resource, *Desc };
		m_CubeMapOffsets[name] = m_CubeHeapOffset;
		m_CubeHeapOffset++;
		return &m_CubeSrvResources[name];
	case D3D12_SRV_DIMENSION_TEXTURE2D:
		m_SrvResources[name] = SrvResourcePack{ name, resource, *Desc };
		m_SrvUavOffsets[name] = m_SrvHeapOffset;
		m_SrvHeapOffset++;
		return &m_SrvResources[name];
	default:
		return nullptr;
	}

}
UavResourcePack* ResourceManager::AddUav(const string& name, ID3D12Resource* resource, ID3D12Resource* Counterresource, D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc)
{
	if (m_SrvUavOffsets.end() != m_SrvUavOffsets.find(name))
	{
		// 예외 처리
		return nullptr;
	}
	// 리소스 저장
	m_UavResources[name]  = UavResourcePack{name, resource, Counterresource, *Desc };
	m_SrvUavOffsets[name] = m_SrvHeapOffset;
	m_SrvHeapOffset++;

	return &m_UavResources[name];
}
RenderTargetPack* ResourceManager::AddRtv(const string& name, ID3D12Resource* RenderResource, D3D12_RENDER_TARGET_VIEW_DESC* Desc)
{
	if (m_RenderTargets.end() != m_RenderTargets.find(name))
	{
		return nullptr;
	}
	// 렌더 타겟 정보 저장
	(Desc != nullptr) ?
		m_RenderTargets[name] = RenderTargetPack{ name, RenderResource, *Desc, } :
		m_RenderTargets[name] = RenderTargetPack{ name, RenderResource, D3D12_RENDER_TARGET_VIEW_DESC(),
	CD3DX12_CPU_DESCRIPTOR_HANDLE(), true};

	m_RtvOffsets[name] = m_RtvHeapOffset;
	m_RtvHeapOffset++;

	return &m_RenderTargets[name];
}
DepthStencilViewPack* ResourceManager::AddDsv(const string& name, ID3D12Resource* RenderResource, D3D12_DEPTH_STENCIL_VIEW_DESC* Desc)
{
	if (m_DepthStencilViews.end() != m_DepthStencilViews.find(name))
	{
		return nullptr;
	}
	(Desc != nullptr) ?
		m_DepthStencilViews[name] = DepthStencilViewPack{ name, RenderResource, *Desc } :
		m_DepthStencilViews[name] = DepthStencilViewPack{ name, RenderResource, D3D12_DEPTH_STENCIL_VIEW_DESC(),
	CD3DX12_CPU_DESCRIPTOR_HANDLE(), true };

	m_DsvOffsets[name] = m_DsvHeapOffset;
	m_DsvHeapOffset++;

	return &m_DepthStencilViews[name];
}
JGRCObject*     ResourceManager::AddJGRCObject(JGMaterial* mat, JGBaseMesh* mesh, const string& meshname, EObjType type)
{
	JGRCObject* result = nullptr;
	auto obj = make_unique<JGRCObject>(++m_ObjectCBIndex, type);
	obj->SetMaterial(mat);
	obj->SetMesh(mesh, meshname);
	result = obj.get();

	m_JGRCObjectMems.push_back(move(obj));
	return result;
}
JGMaterial*     ResourceManager::AddMaterial(const MaterialDesc& desc)
{
	auto mat = make_unique<JGMaterial>(++m_MaterialCBIndex, desc);
	JGMaterial* result = mat.get();

	m_JGMaterialMems.push_back(move(mat));
	return result;
}
JGStaticMesh*   ResourceManager::AddStaticMesh(const string& name)
{
	auto mesh = make_unique<JGStaticMesh>(name);
	JGStaticMesh* result = mesh.get();

	m_MeshMems.push_back(move(mesh));
	return result;
}
JGSkeletalMesh* ResourceManager::AddSkeletalMesh(const string& name)
{
	auto mesh = make_unique<JGSkeletalMesh>(name);
	JGSkeletalMesh* result = mesh.get();

	m_MeshMems.push_back(move(mesh));
	return result;
}
PassData*       ResourceManager::AddPassData()
{
	auto passData = make_unique<PassData>();
	passData->PassCBIndex = ++m_PassCBIndex;
	PassData* result = passData.get();
	m_PassDataMems.push_back(move(passData));
	return result;
}
SrvResourcePack* ResourceManager::SetSrv(const string& name, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* Desc)
{
	if (m_SrvUavOffsets.end() == m_SrvUavOffsets.find(name))
	{
		// 예외 처리
		return nullptr;
	}
	if (Desc == nullptr)
	{
		m_Device->CreateShaderResourceView(
			resource,
			&m_SrvResources[name].Desc,
			GetCPUSrvUavHandle(m_SrvUavOffsets[name]));
	}
	else
	{
		m_Device->CreateShaderResourceView(
			resource,
			Desc,
			GetCPUSrvUavHandle(m_SrvUavOffsets[name]));
		m_SrvResources[name].Desc = *Desc;
	}
	m_SrvResources[name].Resource = resource;
	return &m_SrvResources[name];
}
UavResourcePack* ResourceManager::SetUav(const string& name, ID3D12Resource* resource, ID3D12Resource* Counterresource  ,D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc)
{
	if (m_SrvUavOffsets.end() == m_SrvUavOffsets.find(name))
	{
		// 예외 처리
		return nullptr;
	}
	if (Desc == nullptr)
	{
		m_Device->CreateUnorderedAccessView(
			resource,
			Counterresource,
			&m_UavResources[name].Desc,
			GetCPUSrvUavHandle(m_SrvUavOffsets[name]));
	}
	else
	{
		m_Device->CreateUnorderedAccessView(
			resource,
			Counterresource,
			Desc,
			GetCPUSrvUavHandle(m_SrvUavOffsets[name]));
		m_UavResources[name].Desc = *Desc;
	}
	m_UavResources[name].Resource = resource;
	m_UavResources[name].CounterResource = Counterresource;
	return &m_UavResources[name];
}
RenderTargetPack* ResourceManager::SetRtv(const string& name, ID3D12Resource* RenderResource, D3D12_RENDER_TARGET_VIEW_DESC* Desc)
{
	if (m_RenderTargets.end() == m_RenderTargets.find(name))
	{
		// 예외 처리
		return nullptr;
	}
	if (Desc == nullptr)
	{
		m_Device->CreateRenderTargetView(
			RenderResource,
			&m_RenderTargets[name].Desc,
			m_RenderTargets[name].Handle);
	}
	else
	{
		m_Device->CreateRenderTargetView(
			RenderResource,
			Desc,
			m_RenderTargets[name].Handle);
		m_RenderTargets[name].Desc = *Desc;
	}
	m_RenderTargets[name].RenderResource = RenderResource;
	return &m_RenderTargets[name];
}
DepthStencilViewPack* ResourceManager::SetDsv(const string& name, ID3D12Resource* RenderResource, D3D12_DEPTH_STENCIL_VIEW_DESC* Desc)
{
	if (m_DepthStencilViews.end() == m_DepthStencilViews.find(name))
	{
		// 예외 처리
		return nullptr;
	}
	if (Desc == nullptr)
	{
		m_Device->CreateDepthStencilView(
			RenderResource,
			&m_DepthStencilViews[name].Desc,
			m_DepthStencilViews[name].Handle);
	}
	else
	{
		m_Device->CreateDepthStencilView(
			RenderResource,
			Desc,
			m_DepthStencilViews[name].Handle);
		m_DepthStencilViews[name].Desc = *Desc;
	}
	m_DepthStencilViews[name].RenderResource = RenderResource;
	return &m_DepthStencilViews[name];
}
void ResourceManager::BuildResourceManager(ID3D12GraphicsCommandList* CommandList, CommonShaderRootSignature* RootSig)
{
	BuildResourceData(CommandList, RootSig);
	BuildResourceHeap();
}
void ResourceManager::BuildResourceData(ID3D12GraphicsCommandList* CommandList, CommonShaderRootSignature* RootSig)
{
	for (auto& obj : m_JGRCObjectMems)
	{
		obj->Build(CommandList, RootSig);
	}
	for (auto& mesh : m_MeshMems)
		mesh->CreateMesh(CommandList);
}
void ResourceManager::BuildResourceHeap()
{
	// CBV, SRV, UAV Heap
	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
	HeapDesc.NumDescriptors = m_SrvHeapOffset + m_CubeHeapOffset;
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&HeapDesc,
		IID_PPV_ARGS(m_SrvDescriptorHeap.GetAddressOf())));
	// RenderTarget Heap
	HeapDesc.NumDescriptors = m_RtvHeapOffset;
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HeapDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&HeapDesc,
		IID_PPV_ARGS(m_RtvDescriptorHeap.GetAddressOf())));
	// DepthStencilView Heap
	HeapDesc.NumDescriptors = m_DsvHeapOffset;
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HeapDesc.NodeMask = 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&HeapDesc,
		IID_PPV_ARGS(m_DsvDescriptorHeap.GetAddressOf())));



	// 텍스쳐 
	for (auto& tex : m_Textures)
	{
		m_Device->CreateShaderResourceView(
			tex.second.Texture->Resource.Get(),
			&GetDesc(&(tex.second)),
			GetCPUSrvUavHandle(m_TextureOffsets[tex.first])
		);
	}
	// ShaderResourceView
	for (auto& srv : m_SrvResources)
	{
		m_Device->CreateShaderResourceView(
			srv.second.Resource,
			&srv.second.Desc,
			GetCPUSrvUavHandle(m_SrvUavOffsets[srv.first])
		);
	}

	// UnorderdAccessView
	for (auto& uav : m_UavResources)
	{
		m_Device->CreateUnorderedAccessView(
			uav.second.Resource,
			uav.second.CounterResource,
			&uav.second.Desc,
			GetCPUSrvUavHandle(m_SrvUavOffsets[uav.first])
		);
	}
	// 큐브 맵
	for (auto& cubetex : m_CubeTextures)
	{
		m_Device->CreateShaderResourceView(
			cubetex.second.Texture->Resource.Get(),
			&GetDesc(&(cubetex.second)),
			GetCPUSrvUavHandle(m_SrvHeapOffset + m_CubeTextureOffsets[cubetex.first])
		);
	}
	for (auto& cubesrv : m_CubeSrvResources)
	{
		m_Device->CreateShaderResourceView(
			cubesrv.second.Resource,
			&cubesrv.second.Desc,
			GetCPUSrvUavHandle(m_SrvHeapOffset + m_CubeMapOffsets[cubesrv.first])
		);
	}
	// RenderTargetView
	for (auto& rtv : m_RenderTargets)
	{
		rtv.second.Handle  = GetCPURtvHandle(m_RtvOffsets[rtv.second.PackName]);
		ID3D12Resource* resource = rtv.second.RenderResource;
		(rtv.second.bDefault) ?
			m_Device->CreateRenderTargetView(resource, nullptr, rtv.second.Handle) :
			m_Device->CreateRenderTargetView(resource, &rtv.second.Desc, rtv.second.Handle);
	}
	// DepthStencilView
	for (auto& dsv : m_DepthStencilViews)
	{
		dsv.second.Handle = GetCPUDsvHandle(m_DsvOffsets[dsv.second.PackName]);
		ID3D12Resource* resource = dsv.second.RenderResource;
		(dsv.second.bDefault) ?
			m_Device->CreateDepthStencilView(resource, nullptr, dsv.second.Handle) :
			m_Device->CreateDepthStencilView(resource, &dsv.second.Desc, dsv.second.Handle);
	}

}
Texture* ResourceManager::GetTexture(const wstring& path)
{
	if (m_Textures.end() != m_Textures.find(path))
	{
		return m_Textures[path].Texture.get();
	}
	if (m_CubeTextures.end() != m_CubeTextures.find(path))
	{
		return m_CubeTextures[path].Texture.get();
	}
	// 예외처리
	return nullptr;
}
ID3D12Resource* ResourceManager::GetSrv(const string& name)
{
	if (m_SrvResources.end() != m_SrvResources.find(name))
	{
		return m_SrvResources[name].Resource;
	}
	if (m_CubeSrvResources.end() != m_CubeSrvResources.find(name))
	{
		return m_CubeSrvResources[name].Resource;
	}
	return nullptr;
}
ID3D12Resource* ResourceManager::GetUrv(const string& name)
{
	if (m_UavResources.end() != m_UavResources.find(name))
	{
		return m_UavResources[name].Resource;
	}
	return nullptr;
}
CD3DX12_CPU_DESCRIPTOR_HANDLE ResourceManager::GetRtvHandle(const string& name)
{
	if (m_RtvOffsets.end() == m_RtvOffsets.find(name))
		return CD3DX12_CPU_DESCRIPTOR_HANDLE();
	return GetCPURtvHandle(m_RtvOffsets[name]);
}
CD3DX12_CPU_DESCRIPTOR_HANDLE ResourceManager::GetDsvHandle(const string& name)
{
	if (m_DsvOffsets.end() == m_DsvOffsets.find(name))
		return CD3DX12_CPU_DESCRIPTOR_HANDLE();
	return GetCPUDsvHandle(m_DsvOffsets[name]);
}
TexturePack* ResourceManager::GetTexturePack(const std::wstring& path)
{
	if (m_Textures.end() != m_Textures.find(path))
		return &m_Textures[path];
	if (m_CubeTextures.end() != m_CubeTextures.find(path))
		return &m_CubeTextures[path];
	return nullptr;
}
SrvResourcePack* ResourceManager::GetSrvPack(const std::string& name)
{
	if (m_SrvResources.end() != m_SrvResources.find(name))
		return &m_SrvResources[name];
	if (m_CubeSrvResources.end() != m_CubeSrvResources.find(name))
		return &m_CubeSrvResources[name];
	return nullptr;
}
UavResourcePack* ResourceManager::GetUavPack(const std::string& name)
{
	if (m_UavResources.end() == m_UavResources.find(name))
		return nullptr;
	return &m_UavResources[name];
}
RenderTargetPack* ResourceManager::GetRtvPack(const string& name)
{
	if (m_RenderTargets.end() == m_RenderTargets.find(name))
		return nullptr;
	return &m_RenderTargets[name];
}
DepthStencilViewPack* ResourceManager::GetDsvPack(const string& name)
{
	if (m_DepthStencilViews.end() == m_DepthStencilViews.find(name))
		return nullptr;
	return &m_DepthStencilViews[name];
}
UINT ResourceManager::GetTextureIndex(const wstring& path)
{
	if (m_TextureOffsets.end() != m_TextureOffsets.find(path))
	{
		return m_TextureOffsets[path];
	}
	if (m_CubeTextureOffsets.end() != m_CubeTextureOffsets.find(path))
	{
		return m_SrvHeapOffset + m_CubeTextureOffsets[path];
	}
	// 예외 처리
	return -1;
}
UINT ResourceManager::GetSrvUavIndex(const string& name)
{
	if (m_SrvUavOffsets.end() != m_SrvUavOffsets.find(name))
	{
		return m_SrvUavOffsets[name];
	}
	if (m_CubeMapOffsets.end() != m_CubeMapOffsets.find(name))
	{
		return m_SrvHeapOffset + m_CubeMapOffsets[name];
	}
	// 예외 처리
	return -1;
}
UINT ResourceManager::GetRtvIndex(const string& name)
{
	if (m_RtvOffsets.end() == m_RtvOffsets.find(name))
		return -1;
	return m_RtvOffsets[name];
}
UINT ResourceManager::GetDsvIndex(const string& name)
{
	if (m_DsvOffsets.end() == m_DsvOffsets.find(name))
		return -1;
	return m_DsvOffsets[name];
}
UINT ResourceManager::GetCubeMapShaderIndex(const std::string& name)
{
	if (m_CubeMapOffsets.end() != m_CubeMapOffsets.find(name))
	{
		return m_CubeMapOffsets[name];
	}
	// 예외 처리
	return -1;
}
UINT ResourceManager::GetCubeTextureShaderIndex(const std::wstring& name)
{
	if (m_CubeTextureOffsets.end() != m_CubeTextureOffsets.find(name))
	{
		return m_CubeTextureOffsets[name];
	}
	// 예외 처리
	return -1;
}
CD3DX12_GPU_DESCRIPTOR_HANDLE ResourceManager::GetGPUSrvUavHandle(const UINT HeapIndex)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	handle.Offset(HeapIndex, m_Core->CbvSrvUavDescriptorSize());

	return handle;
}
CD3DX12_CPU_DESCRIPTOR_HANDLE ResourceManager::GetCPUSrvUavHandle(const UINT HeapIndex)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(HeapIndex, m_Core->CbvSrvUavDescriptorSize());

	return handle;
}
CD3DX12_CPU_DESCRIPTOR_HANDLE ResourceManager::GetCPURtvHandle(UINT HeapIndex)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(HeapIndex, m_Core->RtvDescriptorSize());
	return handle;
}
CD3DX12_CPU_DESCRIPTOR_HANDLE ResourceManager::GetCPUDsvHandle(UINT HeapIndex)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(HeapIndex, m_Core->DsvDescriptorSize());
	return handle;
}
CD3DX12_GPU_DESCRIPTOR_HANDLE ResourceManager::GetGPUTexture2DHandle()
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}
CD3DX12_GPU_DESCRIPTOR_HANDLE ResourceManager::GetGPUCubeMapHandle()
{
	return GetGPUSrvUavHandle(m_SrvHeapOffset);
}
UINT ResourceManager::JGRCObjectSize()  const
{
	return (UINT)m_JGRCObjectMems.size();
}
UINT ResourceManager::JGMaterialSize()  const
{
	return (UINT)m_JGMaterialMems.size();
}
UINT ResourceManager::PassDataSize()    const
{
	return (UINT)m_PassDataMems.size();
}
UINT ResourceManager::SkinnedDataSize() const
{
	return (UINT)JGRCObject::SkinnedCount();
}
void ResourceManager::Clear()
{
	m_Textures.clear();
	m_SrvResources.clear();
	m_UavResources.clear();
	m_TextureOffsets.clear();
	m_SrvUavOffsets.clear();
	m_SrvDescriptorHeap.Reset();
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	m_SrvHeapOffset = 0;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&srvHeapDesc,
		IID_PPV_ARGS(m_SrvDescriptorHeap.GetAddressOf())));
}
bool ResourceManager::ResourceStateTransition(ID3D12GraphicsCommandList* CommandList,
	ID3D12Resource* resource, D3D12_RESOURCE_STATES State)
{
	if (m_ResourceStates.end() == m_ResourceStates.find(resource))
		return false;
	
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		resource, m_ResourceStates[resource], State));
	m_ResourceStates[resource] = State;
	return true;
}
D3D12_SHADER_RESOURCE_VIEW_DESC ResourceManager::GetDesc(TexturePack* t)
{

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = t->Texture->Resource->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = t->Texture->Resource->GetDesc().MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	switch (t->Type)
	{
	case ETextureType::Default:
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		break;
	case ETextureType::Cube:
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		break;
	}
	return srvDesc;
}