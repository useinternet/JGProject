#include "PCH/PCH.h"
#include "Misc/Module.h"
#include "JGGraphics.h"
#include "DirectX12API.h"
#include "Classes/CommandQueue.h"
#include "Classes/ResourceStateTracker.h"
#include "Classes/DescriptionAllocator.h"
#include "Classes/CommandList.h"
#include "DirectX12/DX12FrameBuffer.h"
#include "DirectX12/DX12Texture.h"
#include "DirectX12/DX12JGGui.h"

PDirectX12API::~PDirectX12API()
{
	_commandQueue->Flush();
	_gui.Reset();
}

void PDirectX12API::Initialize(const HJGGraphicsArguments& args)
{
	JG_LOG(Graphics, ELogLevel::Trace, "DirectX12 Init Start");
	_arguments   = args;
	_dx12Factory = HDirectX12Helper::CreateDXGIFactory();

	DXGI_ADAPTER_DESC1 adapterDesc = {};
	_bIsSupportedRayTracing = false;
	_dx12Device = HDirectX12Helper::CreateD3DDevice(_dx12Factory, false, &adapterDesc, &_bIsSupportedRayTracing);

	PString adapterDescription;
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		HRawString str = converterX.to_bytes(adapterDesc.Description);
		adapterDescription = str.c_str();
	}

	if (_dx12Device)
	{
		JG_LOG(Graphics, ELogLevel::Trace, "Success Create D3D12Device");
		JG_LOG(Graphics, ELogLevel::Info, "Description : %s", adapterDescription);
		JG_LOG(Graphics, ELogLevel::Info, "VideoMemory : %d  MB", adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	}
	else
	{
		JG_LOG(Graphics, ELogLevel::Critical, "Failed Create D3D12Device");
	}

	JG_LOG(Graphics, ELogLevel::Trace, "Create DescriptorAllocator...");
	_csuAllocator = Allocate<PDescriptionAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_rtvAllocator = Allocate<PDescriptionAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_dsvAllocator = Allocate<PDescriptionAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	JG_LOG(Graphics, ELogLevel::Trace, "Create CommandQueue...");
	_commandQueue = Allocate<PCommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);

	JG_LOG(Graphics, ELogLevel::Trace, "Create FrameBuffer...");
	HFrameBufferInfo frameBufferInfo;
	frameBufferInfo.Handle = args.Handle;
	frameBufferInfo.ClearColor = args.ClearColor;
	frameBufferInfo.FrameBufferCount = args.BufferCount;
	frameBufferInfo.Width  = args.Width;
	frameBufferInfo.Height = args.Height;
	frameBufferInfo.Format = ETextureFormat::R16G16B16A16_Float;
	_frameBuffer = Allocate<PDX12FrameBuffer>();
	_frameBuffer->Initialize(frameBufferInfo);
	
	_gui = Allocate<PDX12JGGui>();

	GCoreSystem::GetGlobalValues().GraphicsAPI = this;
	JG_LOG(Graphics, ELogLevel::Trace, "DirectX12 Init End");
}

void PDirectX12API::BeginFrame()
{
	//mCacheGraphicsContextDic.clear();
	//mCacheComputeContextDic.clear();

	_commandQueue->Begin();
	_gui->NewFrame();
}

void PDirectX12API::EndFrame()
{
	_frameBuffer->Update();
	_commandQueue->End();
	_frameBuffer->Present();
	_csuAllocator->UpdatePage();
	_rtvAllocator->UpdatePage();
	_dsvAllocator->UpdatePage();
}

IJGGui* PDirectX12API::GetGui() const
{
	if (_gui.IsValid() == false)
	{
		return nullptr;
	}

	return _gui.GetRawPointer();
}

PSharedPtr<ITexture> PDirectX12API::CreateTexture(const HTextureInfo& textureInfo)
{
	PSharedPtr<PDX12Texture> texture = Allocate<PDX12Texture>();
	texture->Initialize(textureInfo);

	return texture;
}

PSharedPtr<ITexture> PDirectX12API::CreateTexture(const char* pixels, const HTextureInfo& textureInfo)
{
	PSharedPtr<PDX12Texture> texture = Allocate<PDX12Texture>();
	texture->InitializeByMemory(pixels, textureInfo);

	return texture;
}

const HJGGraphicsArguments& PDirectX12API::GetArguments() const
{
	return _arguments;
}

HDX12ComPtr<HDX12Resource> PDirectX12API::CreateCommittedResource(
	const PString& name,
	const D3D12_HEAP_PROPERTIES* pHeapProperties,
	D3D12_HEAP_FLAGS heapFlags,
	const D3D12_RESOURCE_DESC* pDesc,
	D3D12_RESOURCE_STATES initialResourceState,
	const D3D12_CLEAR_VALUE* pOptimizedClearValue)
{
	HDX12ComPtr<HDX12Resource> resultResource;
	HRESULT hResult = S_OK;
	{
		HLockGuard<HMutex> lock(_deviceMutex);
		hResult = GetDevice()->CreateCommittedResource(pHeapProperties, heapFlags, pDesc, initialResourceState, pOptimizedClearValue, IID_PPV_ARGS(resultResource.GetAddressOf()));
	}
	if (SUCCEEDED(hResult))
	{
		PResourceStateTracker::RegisterResource(name, resultResource.Get(), initialResourceState);
	}

	return resultResource;
}

void PDirectX12API::DestroyCommittedResource(HDX12ComPtr<HDX12Resource> resource)
{
	if (resource == nullptr)
	{
		return;
	}

	PResourceStateTracker::UnRegisterResource(resource.Get());
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

PSharedPtr<PGraphicsCommandList> PDirectX12API::RequestGraphicsCommandList()
{
	return Cast<PGraphicsCommandList>(_commandQueue->RequestCommandList(ECommandListType::Graphics));
}

PSharedPtr<PComputeCommandList>  PDirectX12API::RequestComputeCommandList()
{
	return Cast<PComputeCommandList>(_commandQueue->RequestCommandList(ECommandListType::Compute));
}

PSharedPtr<PCommandList> PDirectX12API::RequestCommandList()
{
	return _commandQueue->RequestCommandList(ECommandListType::Base);
}

HDescriptionAllocation PDirectX12API::RTVAllocate()
{
	return _rtvAllocator->Allocate();
}

HDescriptionAllocation PDirectX12API::DSVAllocate()
{
	return _dsvAllocator->Allocate();
}

HDescriptionAllocation PDirectX12API::CSUAllocate()
{
	return _csuAllocator->Allocate();
}

PSharedPtr<PCommandQueue> PDirectX12API::GetCommandQueue() const
{
	return _commandQueue;
}

PSharedPtr<PDX12FrameBuffer> PDirectX12API::GetFrameBuffer() const
{
	return _frameBuffer;
}

HDX12Device* HDirectXAPI::GetDevice()
{
	return getDX12API()->GetDevice();
}

HDX12Factory* HDirectXAPI::GetFactory()
{
	return getDX12API()->GetFactory();
}

const HJGGraphicsArguments& HDirectXAPI::GetArguments()
{
	return getDX12API()->GetArguments();
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

PSharedPtr<PGraphicsCommandList> HDirectXAPI::RequestGraphicsCommandList()
{
	return getDX12API()->RequestGraphicsCommandList();
}

PSharedPtr<PComputeCommandList> HDirectXAPI::RequestComputeCommandList()
{
	return getDX12API()->RequestComputeCommandList();
}

PSharedPtr<PCommandList> HDirectXAPI::RequestCommandList()
{
	return getDX12API()->RequestCommandList();
}

HDescriptionAllocation HDirectXAPI::RTVAllocate()
{
	return getDX12API()->RTVAllocate();
}

HDescriptionAllocation HDirectXAPI::DSVAllocate()
{
	return getDX12API()->DSVAllocate();
}

HDescriptionAllocation HDirectXAPI::CSUAllocate()
{
	return getDX12API()->CSUAllocate();
}

PSharedPtr<PCommandQueue> HDirectXAPI::GetCommandQueue()
{
	return getDX12API()->GetCommandQueue();
}

PSharedPtr<PDX12FrameBuffer> HDirectXAPI::GetFrameBuffer()
{
	return getDX12API()->GetFrameBuffer();
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
