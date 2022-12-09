#include "PCH/PCH.h"
#include "Misc/Module.h"
#include "JGGraphics.h"
#include "DirectX12API.h"


HDX12ComPtr<HDX12Resource> PDirectX12API::CreateCommittedResource(
	const PString& name,
	const D3D12_HEAP_PROPERTIES* pHeapProperties,
	D3D12_HEAP_FLAGS heapFlags,
	const D3D12_RESOURCE_DESC* pDesc,
	D3D12_RESOURCE_STATES initialResourceState,
	const D3D12_CLEAR_VALUE* pOptimizedClearValue)
{
	return nullptr;
}

void PDirectX12API::DestroyCommittedResource(HDX12ComPtr<HDX12Resource> resource)
{

}

const HHashMap<uint64, HDX12ComPtr<HDX12RootSignature>>& PDirectX12API::GetRootSignatureCache() const
{
	return _rootSignatureCache;
}

HHashMap<uint64, HDX12ComPtr<HDX12RootSignature>>& PDirectX12API::GetRootSignatureCacheRef()
{
	return _rootSignatureCache;
}

const HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& PDirectX12API::GetGraphicsPSOCache() const
{
	return _graphicsPSOCache;
}

HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& PDirectX12API::GetGraphicsPSOCacheRef()
{
	return _graphicsPSOCache;
}

const HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& PDirectX12API::GetComputePSOCache() const
{
	return _computePSOCache;
}

HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& PDirectX12API::GetComputePSOCacheRef()
{
	return _computePSOCache;
}

const HHashMap<HDX12Resource*, HResourceInfo>& PDirectX12API::GetResourceRefCache() const
{
	return _resourceRefCache;
}

HHashMap<HDX12Resource*, HResourceInfo>& PDirectX12API::GetResourceRefCacheRef()
{
	return _resourceRefCache;
}

uint64 PDirectX12API::GetCurrentFrameIndex() const
{
	return 0;
}

HDX12Device* HDirectXAPI::GetDevice()
{
	return getDX12API()->GetDevice();
}

HDX12ComPtr<HDX12Resource> HDirectXAPI::CreateCommittedResource(const PString& name, const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES initialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue)
{
	return getDX12API()->CreateCommittedResource(name, pHeapProperties, heapFlags, pDesc, initialResourceState, pOptimizedClearValue);
}

void HDirectXAPI::DestroyCommittedResource(HDX12ComPtr<HDX12Resource> resource)
{
	return getDX12API()->DestroyCommittedResource(resource);
}

const HHashMap<uint64, HDX12ComPtr<HDX12RootSignature>>& HDirectXAPI::GetRootSignatureCache()
{
	return getDX12API()->GetRootSignatureCache();
}

HHashMap<uint64, HDX12ComPtr<HDX12RootSignature>>& HDirectXAPI::GetRootSignatureCacheRef()
{
	return getDX12API()->GetRootSignatureCacheRef();
}

const HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& HDirectXAPI::GetGraphicsPSOCache()
{
	return getDX12API()->GetGraphicsPSOCache();
}

HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& HDirectXAPI::GetGraphicsPSOCacheRef()
{
	return getDX12API()->GetGraphicsPSOCacheRef();
}

const HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& HDirectXAPI::GetComputePSOCache()
{
	return getDX12API()->GetComputePSOCache();
}

HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& HDirectXAPI::GetComputePSOCacheRef()
{
	return getDX12API()->GetComputePSOCacheRef();
}

const HHashMap<HDX12Resource*, HResourceInfo>& HDirectXAPI::GetResourceRefCache()
{
	return getDX12API()->GetResourceRefCache();
}

HHashMap<HDX12Resource*, HResourceInfo>& HDirectXAPI::GetResourceRefCacheRef()
{
	return getDX12API()->GetResourceRefCacheRef();
}

uint64 HDirectXAPI::GetCurrentFrameIndex()
{
	return getDX12API()->GetCurrentFrameIndex();
}

PDirectX12API* HDirectXAPI::getDX12API()
{
	static PDirectX12API* cachedAPI = nullptr;

	if (cachedAPI == nullptr)
	{
		HJGGraphicsModule* GraphicsModule = GModuleGlobalSystem::GetInstance().FindModule<HJGGraphicsModule>();
		if (GraphicsModule == nullptr)
		{
			return nullptr;
		}

		PSharedPtr<PJGGraphicsAPI> GraphicsAPI = GraphicsModule->GetGraphicsAPI();
		PSharedPtr<PDirectX12API>  Dx12API = RawFastCast<PDirectX12API>(GraphicsAPI);
		if (Dx12API.IsValid() == false)
		{
			return nullptr;
		}

		cachedAPI = Dx12API.GetRawPointer();
	}

	return cachedAPI;
}
