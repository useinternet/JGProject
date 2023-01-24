#pragma once
#include "JGGraphicsAPI.h"
#include "DirectX12Define.h"
#include "DirectX12/Classes/DirectX12Helper.h"
#include "DirectX12/Classes/ResourceStateTracker.h"


class PCommandQueue;
class PDescriptionAllocator;
class PDX12GraphicsShader;
class PDX12ComputeShader;
class PGraphicsCommandList;
class PComputeCommandList;
class PCommandList;
class PDX12FrameBuffer;
class HDescriptionAllocation;
class PDX12JGGui;

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

	HMutex _deviceMutex;

	PSharedPtr<PDX12FrameBuffer> _frameBuffer;
	PSharedPtr<PDX12JGGui> _gui;

	HJGGraphicsArguments _arguments;
	bool _bIsSupportedRayTracing;
public:
	virtual ~PDirectX12API() = default;

protected:
	virtual void Initialize(const HJGGraphicsArguments& args) override;
	virtual void Destroy() override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
public:
	virtual IJGGui* GetGui() const override;
	virtual PSharedPtr<ITexture> CreateTexture(const HTextureInfo& textureInfo) override;
	virtual PSharedPtr<ITexture> CreateTexture(const char* pixels, const HTextureInfo& textureInfo) override;

public:
	HDX12Device*  GetDevice() const { return _dx12Device.Get(); }
	HDX12Factory* GetFactory() const { return _dx12Factory.Get(); }

	const HJGGraphicsArguments& GetArguments() const;

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

	PSharedPtr<PGraphicsCommandList> RequestGraphicsCommandList();
	PSharedPtr<PComputeCommandList>  RequestComputeCommandList();
	PSharedPtr<PCommandList> RequestCommandList();

	HDescriptionAllocation RTVAllocate();
	HDescriptionAllocation DSVAllocate();
	HDescriptionAllocation CSUAllocate();

	PSharedPtr<PCommandQueue> GetCommandQueue() const;
	PSharedPtr<PDX12FrameBuffer> GetFrameBuffer() const;
};


class HDirectXAPI
{
public:
	static HDX12Device*  GetDevice();
	static HDX12Factory* GetFactory();
	static const HJGGraphicsArguments& GetArguments();

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
	
	static PSharedPtr<PGraphicsCommandList> RequestGraphicsCommandList();
	static PSharedPtr<PComputeCommandList>  RequestComputeCommandList();
	static PSharedPtr<PCommandList>  RequestCommandList();
	static HDescriptionAllocation RTVAllocate();
	static HDescriptionAllocation DSVAllocate();
	static HDescriptionAllocation CSUAllocate();
	static PSharedPtr<PCommandQueue> GetCommandQueue();
	static PSharedPtr<PDX12FrameBuffer> GetFrameBuffer();
private:
	static PDirectX12API* getDX12API();
};