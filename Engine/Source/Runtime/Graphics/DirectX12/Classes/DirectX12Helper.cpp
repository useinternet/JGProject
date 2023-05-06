#include "PCH/PCH.h"
#include "DirectX12Helper.h"


HDX12ComPtr<HDX12Factory> HDirectX12Helper::CreateDXGIFactory()
{
	HRESULT hr = S_OK;
	HDX12ComPtr<HDX12Factory> factory;
	UINT32 dxgiFactoryFlags = 0;

#ifdef _DEBUG
	HDX12ComPtr<ID3D12Debug> debugInterface;
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
	assert(SUCCEEDED(hr) && "failed DebugInterface");

	debugInterface->EnableDebugLayer();

	HDX12ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
	{
		dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
	}
#endif
	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf()));
	assert(SUCCEEDED(hr) && "failed Create DXGIFactory");


	return factory;
}

HDX12ComPtr<HDX12Device>  HDirectX12Helper::CreateD3DDevice(HDX12ComPtr<HDX12Factory> factory, bool bUseWrapDevice, DXGI_ADAPTER_DESC1* OutadapterDesc, bool* IsSupportedRayTracing)
{
	if (factory == nullptr)
	{
		return nullptr;
	}

	HRESULT hr = S_OK;
	if (bUseWrapDevice)
	{
		HDX12ComPtr<IDXGIAdapter>   warpAdapter;
		HDX12ComPtr<HDX12Device>  pDevice;
		hr = factory->EnumWarpAdapter(IID_PPV_ARGS(warpAdapter.GetAddressOf()));
		assert(SUCCEEDED(hr) && "failed Create warpAdapter");

		hr = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(pDevice.GetAddressOf()));
		assert(SUCCEEDED(hr) && "failed Create D3D12Device");

		return pDevice;
	}
	else
	{
		HDX12ComPtr<IDXGIAdapter1>  pAdapter;
		HDX12ComPtr<ID3D12Device5>  pDevice;

		DXGI_ADAPTER_DESC1 adapterDesc = {};
		SIZE_T maxSize = 0;


		HDX12ComPtr<IDXGIAdapter1>  pTempAdapter;
		HDX12ComPtr<ID3D12Device5>  pTempDevice;

		for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(Idx, &pTempAdapter); ++Idx)
		{
			DXGI_ADAPTER_DESC1 desc;
			pTempAdapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			if (desc.DedicatedVideoMemory > maxSize && SUCCEEDED(D3D12CreateDevice(pTempAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pTempDevice))))
			{
				pTempAdapter->GetDesc1(&desc);

				maxSize     = desc.DedicatedVideoMemory;
				adapterDesc = desc;
				if (IsSupportedRayTracing)
				{
					*IsSupportedRayTracing = IsDirectXRaytracingSupported(pTempAdapter.Get());
				}

				pDevice  = pTempDevice;
				pAdapter = pTempAdapter;
			}
		}

		if (OutadapterDesc && maxSize > 0)
		{
			*OutadapterDesc = adapterDesc;
		}
		else if (maxSize == 0)
		{
			return nullptr;
		}
#ifdef _DEBUG
		if (pDevice)
		{

			HDX12ComPtr<ID3D12InfoQueue> infoQueue;
			pDevice.As(&infoQueue);

			D3D12_MESSAGE_ID denyIds[] = {
			  D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
			};
			D3D12_MESSAGE_SEVERITY severities[] = {
			  D3D12_MESSAGE_SEVERITY_INFO
			};
			D3D12_INFO_QUEUE_FILTER filter{};
			filter.DenyList.NumIDs		  = _countof(denyIds);
			filter.DenyList.pIDList		  = denyIds;
			filter.DenyList.NumSeverities = _countof(severities);
			filter.DenyList.pSeverityList = severities;

			infoQueue->PushStorageFilter(&filter);
		}
#endif
		return pDevice;
	}
	
}
HDX12ComPtr<HDX12SwapChain> HDirectX12Helper::CreateDXGISwapChain(HJWHandle handle, HDX12ComPtr<HDX12Factory> factory, HDX12ComPtr<HDX12CommandQueue> cmdQue, DXGI_FORMAT format, uint32_t width, uint32_t height, uint32_t bufferCount)
{
	HRESULT hr = S_OK;
	HDX12ComPtr<HDX12SwapChain> dxgiSwapChain4;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = format;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = 0; //  CheckTearingSupport(factory) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	
	HDX12ComPtr<IDXGISwapChain1> swapChain1;
	hr = factory->CreateSwapChainForHwnd(cmdQue.Get(), handle, &swapChainDesc, nullptr, nullptr, &swapChain1);
	assert(SUCCEEDED(hr) && "failed Create IDXGISwapChain4");
	hr = factory->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER);
	assert(SUCCEEDED(hr) && "failed MakeWindowAssociation");
	hr = swapChain1.As(&dxgiSwapChain4);
	assert(SUCCEEDED(hr) && "failed SwapChain1 As SwapChain4");
	return dxgiSwapChain4;
}

HDX12ComPtr<HDX12CommandQueue> HDirectX12Helper::CreateD3DCommandQueue(HDX12ComPtr<HDX12Device> device, D3D12_COMMAND_LIST_TYPE type)
{
	HRESULT hr = S_OK;
	HDX12ComPtr<HDX12CommandQueue> d3d12CommandQueue;

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type		= type;
	desc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask	= 0;

	hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue));
	assert(SUCCEEDED(hr) && "failed Create ID3D12CommandQueue");
	return d3d12CommandQueue;

}

HDX12ComPtr<HDX12CommandList> HDirectX12Helper::CreateD3DCommandList(HDX12ComPtr<HDX12Device> device, HDX12ComPtr<ID3D12CommandAllocator> cmdAllocator, D3D12_COMMAND_LIST_TYPE type)
{
	HRESULT hr = S_OK;
	HDX12ComPtr<HDX12CommandList> commandList;

	hr = device->CreateCommandList(0, type, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(hr) && "failed Create ID3D12GraphicsCommandList");
	return commandList;
}

HDX12ComPtr<HDX12CommandAllocator> HDirectX12Helper::CreateD3DCommandAllocator(HDX12ComPtr<HDX12Device> device, D3D12_COMMAND_LIST_TYPE type)
{
	HRESULT hr = S_OK;
	HDX12ComPtr<HDX12CommandAllocator> commandAllocator;
	hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));

	assert(SUCCEEDED(hr) && "failed Create ID3D12CommandAllocator");
	return commandAllocator;
}

HDX12ComPtr<HDX12DescriptorHeap> HDirectX12Helper::CreateD3DDescriptorHeap(HDX12ComPtr<HDX12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag, uint32_t numDescriptor)
{
	HRESULT hr = S_OK;
	HDX12ComPtr<HDX12DescriptorHeap> descriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptor;
	desc.Type = type;
	desc.Flags = flag;

	hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
	hr = device->GetDeviceRemovedReason();
	assert(SUCCEEDED(hr) && "failed Create ID3D12DescriptorHeap");

	return descriptorHeap;
}

HDX12ComPtr<HDX12RootSignature> HDirectX12Helper::CreateD3DRootSignature(HDX12ComPtr<HDX12Device> device, CD3DX12_ROOT_SIGNATURE_DESC* DESC)
{
	HDX12ComPtr<HDX12RootSignature> rootSig;
	HDX12ComPtr<ID3DBlob> serializedRootSig;
	HDX12ComPtr<ID3DBlob> errorBlob;

	HRESULT hr = D3D12SerializeRootSignature(
		DESC,
		D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(),
		errorBlob.GetAddressOf());

	PString errstr;
	if (errorBlob != nullptr)
	{
		errstr = (const char*)errorBlob->GetBufferPointer();
	}


	hr = device->CreateRootSignature(
		0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(rootSig.GetAddressOf()));

	return rootSig;
}

bool HDirectX12Helper::IsDirectXRaytracingSupported(HDX12Adapter* adapter)
{
	HDX12ComPtr<HDX12Device> testDevice;
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};

	return SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&testDevice)))
		&& SUCCEEDED(testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData, sizeof(featureSupportData)))
		&& featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}

DXGI_FORMAT HDirectX12Helper::ConvertDXGIFormat(ETextureFormat format)
{
	switch (format)
	{
	case ETextureFormat::R11G11B10_Float:    return DXGI_FORMAT_R11G11B10_FLOAT;
	case ETextureFormat::R8_Unorm:			 return DXGI_FORMAT_R8_UNORM;
	case ETextureFormat::R16_Float:			 return DXGI_FORMAT_R16_FLOAT;
	case ETextureFormat::R32_Float:			 return DXGI_FORMAT_R32_FLOAT;
	case ETextureFormat::R16G16_Float:		 return DXGI_FORMAT_R16G16_FLOAT;
	case ETextureFormat::R8G8B8A8_Unorm:     return DXGI_FORMAT_R8G8B8A8_UNORM;
	case ETextureFormat::R16G16B16A16_Unorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
	case ETextureFormat::R16G16B16A16_Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case ETextureFormat::R32G32B32A32_Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ETextureFormat::R16G16B16A16_Uint:  return DXGI_FORMAT_R16G16B16A16_UINT;
	case ETextureFormat::R8_Uint:			 return DXGI_FORMAT_R8_UINT;
	case ETextureFormat::R16_Uint:			 return DXGI_FORMAT_R16_UINT;
	case ETextureFormat::R32_Uint:			 return DXGI_FORMAT_R32_UINT;
	case ETextureFormat::R24G8_TYPELESS:	 return DXGI_FORMAT_R24G8_TYPELESS;
	case ETextureFormat::D24_Unorm_S8_Uint:  return	DXGI_FORMAT_D24_UNORM_S8_UINT;
	default:
	{
		PSharedPtr<JGEnum> Enum = StaticEnum<ETextureFormat>();
		PName EnumName = Enum->GetEnumNameByIndex((int32)format);

		JG_LOG(Graphics, ELogLevel::Error, "This %s DirectX12 TextureFormat is not supported convert ETextureFormat", EnumName.ToString());
	}
	return DXGI_FORMAT_UNKNOWN;
	}
}

DXGI_FORMAT HDirectX12Helper::ConvertDXGIFormat(EShaderDataType type)
{
	switch (type)
	{
	case EShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
	case EShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
	case EShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
	case EShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;

	case EShaderDataType::Int: return  DXGI_FORMAT_R32_SINT;
	case EShaderDataType::Int2: return DXGI_FORMAT_R32G32_SINT;
	case EShaderDataType::Int3: return DXGI_FORMAT_R32G32B32_SINT;
	case EShaderDataType::Int4: return DXGI_FORMAT_R32G32B32A32_SINT;

	case EShaderDataType::Uint: return DXGI_FORMAT_R32_UINT;
	case EShaderDataType::Uint2: return DXGI_FORMAT_R32G32_UINT;
	case EShaderDataType::Uint3: return DXGI_FORMAT_R32G32B32_UINT;
	case EShaderDataType::Uint4: return DXGI_FORMAT_R32G32B32A32_UINT;

	default:
		JG_LOG(Graphics, ELogLevel::Error, "ConvertShaderDataType :: not supported ShaderDataType");
		return DXGI_FORMAT_UNKNOWN;
	}
}


D3D12_RESOURCE_STATES HDirectX12Helper::ConvertDX12ResourceState(EResourceState state)
{
	switch (state)
	{
	case EResourceState::Common:					return D3D12_RESOURCE_STATE_COMMON;
	case EResourceState::VertexAndConstantBuffer:	return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	case EResourceState::IndexBuffer:				return D3D12_RESOURCE_STATE_INDEX_BUFFER;
	case EResourceState::RenderTarget:				return D3D12_RESOURCE_STATE_RENDER_TARGET;
	case EResourceState::UnorderedAccess:			return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	case EResourceState::DepthWrite:				return D3D12_RESOURCE_STATE_DEPTH_WRITE;
	case EResourceState::DepthRead:					return D3D12_RESOURCE_STATE_DEPTH_READ;
	case EResourceState::NonePixelShaderResource:	return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	case EResourceState::PixelShaderResource:		return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	case EResourceState::CopyDest:					return D3D12_RESOURCE_STATE_COPY_DEST;
	case EResourceState::CopySource:				return D3D12_RESOURCE_STATE_COPY_SOURCE;
	case EResourceState::RayTracingAccelerationStructure: return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	case EResourceState::GenericRead:				return D3D12_RESOURCE_STATE_GENERIC_READ;
	default:
		return D3D12_RESOURCE_STATE_COMMON;

	}
	return D3D12_RESOURCE_STATE_COMMON;
}





