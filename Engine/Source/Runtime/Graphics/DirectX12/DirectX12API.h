#pragma once
#include "JGGraphicsAPI.h"
#include "DirectX12Define.h"
#include "DirectX12/Classes/DirectX12Helper.h"
#include "DirectX12/Classes/ResourceStateTracker.h"


class PCommandQueue;
class PDescriptionAllocator;
class PDX12GraphicsShader;
class PDX12ComputeShader;

class PDirectX12API : public PJGGraphicsAPI
{
	HDX12ComPtr<HDX12Factory> _dx12Factory;
	HDX12ComPtr<HDX12Device>  _dx12Device;

	PSharedPtr<PCommandQueue> _commandQueue;
	PSharedPtr<PDescriptionAllocator> _csuAllocator;
	PSharedPtr<PDescriptionAllocator> _rtvAllocator;
	PSharedPtr<PDescriptionAllocator> _dsvAllocator;

	HHashMap<uint64, PSharedPtr<PDX12GraphicsShader>> _graphicsPSOs;
	HHashMap<uint64, PSharedPtr<PDX12ComputeShader>>  _computePSOs;

	HHashMap<uint64, HDX12ComPtr<HDX12RootSignature>> _rootSignatureCache;
	HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>> _graphicsPSOCache;
	HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>> _computePSOCache;
	HHashMap<HDX12Resource*, HResourceInfo> _resourceRefCache;

	//HHashMap<uint64, SharedPtr<DirectX12FrameBuffer>> mFrameBuffers;
	//const u64 mFrameBufferCount = 3;
	//u64       mFrameBufferIndex = 0;

public:
	virtual ~PDirectX12API() = default;


public:
	HDX12Device* GetDevice() const { return _dx12Device.Get(); }

	HDX12ComPtr<HDX12Resource> CreateCommittedResource(
		const PString& name,
		const D3D12_HEAP_PROPERTIES* pHeapProperties,
		D3D12_HEAP_FLAGS heapFlags,
		const D3D12_RESOURCE_DESC* pDesc,
		D3D12_RESOURCE_STATES initialResourceState,
		const D3D12_CLEAR_VALUE* pOptimizedClearValue);

	void DestroyCommittedResource(HDX12ComPtr<HDX12Resource> resource);

	const HHashMap<uint64, HDX12ComPtr<HDX12RootSignature>>& GetRootSignatureCache() const;
	HHashMap<uint64, HDX12ComPtr<HDX12RootSignature>>& GetRootSignatureCacheRef();

	const HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& GetGraphicsPSOCache() const;
	HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& GetGraphicsPSOCacheRef();

	const HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& GetComputePSOCache() const;
	HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& GetComputePSOCacheRef();

	const HHashMap<HDX12Resource*, HResourceInfo>& GetResourceRefCache() const;
	HHashMap<HDX12Resource*, HResourceInfo>& GetResourceRefCacheRef();

	uint64 GetCurrentFrameIndex() const;
};


class HDirectXAPI
{
public:
	static HDX12Device* GetDevice();

	static 	HDX12ComPtr<HDX12Resource> CreateCommittedResource(
		const PString& name,
		const D3D12_HEAP_PROPERTIES* pHeapProperties,
		D3D12_HEAP_FLAGS heapFlags,
		const D3D12_RESOURCE_DESC* pDesc,
		D3D12_RESOURCE_STATES initialResourceState,
		const D3D12_CLEAR_VALUE* pOptimizedClearValue);

	static void DestroyCommittedResource(HDX12ComPtr<HDX12Resource> resource);

	static const HHashMap<uint64, HDX12ComPtr<HDX12RootSignature>>& GetRootSignatureCache();
	static HHashMap<uint64, HDX12ComPtr<HDX12RootSignature>>& GetRootSignatureCacheRef();
	static const HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& GetGraphicsPSOCache();
	static HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& GetGraphicsPSOCacheRef();
	static const HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& GetComputePSOCache();
	static HHashMap<uint64, HDX12ComPtr<HDX12Pipeline>>& GetComputePSOCacheRef();
	static const HHashMap<HDX12Resource*, HResourceInfo>& GetResourceRefCache();
	static HHashMap<HDX12Resource*, HResourceInfo>& GetResourceRefCacheRef();
	
	static uint64 GetCurrentFrameIndex();
private:
	static PDirectX12API* getDX12API();
};