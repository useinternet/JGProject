#include "pch.h"
#include "DirectX12Helper.h"







namespace JG
{
	Microsoft::WRL::ComPtr<IDXGIFactory4> CreateDXGIFactory()
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
		UINT32 dxgiFactoryFlags = 0;

#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
		assert(SUCCEEDED(hr) && "failed DebugInterface");

		debugInterface->EnableDebugLayer();

		Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
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
	Microsoft::WRL::ComPtr<ID3D12Device5>  CreateD3DDevice(
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory,
		bool is_UseWrapDevice,
		DXGI_ADAPTER_DESC1* OutadapterDesc)
	{
		HRESULT hr = S_OK;
		if (is_UseWrapDevice)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter>  warpAdapter;
			Microsoft::WRL::ComPtr<ID3D12Device5>  pDevice;
			hr = factory->EnumWarpAdapter(IID_PPV_ARGS(warpAdapter.GetAddressOf()));
			assert(SUCCEEDED(hr) && "failed Create warpAdapter");

			hr = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(pDevice.GetAddressOf()));
			assert(SUCCEEDED(hr) && "failed Create D3D12Device");


			return pDevice;
		}
		else
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;
			Microsoft::WRL::ComPtr<ID3D12Device5>  pDevice;

			DXGI_ADAPTER_DESC1 adapterDesc = {};
			SIZE_T maxSize = 0;

			for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(Idx, &pAdapter); ++Idx)
			{
				DXGI_ADAPTER_DESC1 desc;
				pAdapter->GetDesc1(&desc);
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				if (desc.DedicatedVideoMemory > maxSize && SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
				{
					pAdapter->GetDesc1(&desc);
					maxSize = desc.DedicatedVideoMemory;
					adapterDesc = desc;
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

				ComPtr<ID3D12InfoQueue> infoQueue;
				pDevice.As(&infoQueue);

				D3D12_MESSAGE_ID denyIds[] = {
				  D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				};
				D3D12_MESSAGE_SEVERITY severities[] = {
				  D3D12_MESSAGE_SEVERITY_INFO
				};
				D3D12_INFO_QUEUE_FILTER filter{};
				filter.DenyList.NumIDs = _countof(denyIds);
				filter.DenyList.pIDList = denyIds;
				filter.DenyList.NumSeverities = _countof(severities);
				filter.DenyList.pSeverityList = severities;

				infoQueue->PushStorageFilter(&filter);
			}
#endif
			return pDevice;
		}
	}
	Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateDXGISwapChain(
		HWND hWnd,
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQue,
		DXGI_FORMAT format,
		uint32_t width,
		uint32_t height,
		uint32_t bufferCount)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain4;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = format;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = bufferCount;
		swapChainDesc.Scaling	 = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode	 = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = 0; //  CheckTearingSupport(factory) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		hr = factory->CreateSwapChainForHwnd(cmdQue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
		assert(SUCCEEDED(hr) && "failed Create IDXGISwapChain4");
		hr = factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
		assert(SUCCEEDED(hr) && "failed MakeWindowAssociation");
		hr = swapChain1.As(&dxgiSwapChain4);
		assert(SUCCEEDED(hr) && "failed SwapChain1 As SwapChain4");
		return dxgiSwapChain4;
	}



	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CreateD3DCommandQueue(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		D3D12_COMMAND_LIST_TYPE type)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue));
		assert(SUCCEEDED(hr) && "failed Create ID3D12CommandQueue");
		return d3d12CommandQueue;

	}


	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> CreateD3DCommandList(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator,
		D3D12_COMMAND_LIST_TYPE type)
	{

		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList;
		hr = device->CreateCommandList(0, type, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
		assert(SUCCEEDED(hr) && "failed Create ID3D12GraphicsCommandList");
		return commandList;
	}


	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateD3DCommandAllocator(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		D3D12_COMMAND_LIST_TYPE type)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));


		assert(SUCCEEDED(hr) && "failed Create ID3D12CommandAllocator");
		return commandAllocator;
	}



	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateD3DDescriptorHeap(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		D3D12_DESCRIPTOR_HEAP_FLAGS flag,
		uint32_t numDescriptor)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = numDescriptor;
		desc.Type = type;
		desc.Flags = flag;

		hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
		hr = device->GetDeviceRemovedReason();
		assert(SUCCEEDED(hr) && "failed Create ID3D12DescriptorHeap");


		return descriptorHeap;

	}
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateD3DRootSignature(Microsoft::WRL::ComPtr<ID3D12Device> device, CD3DX12_ROOT_SIGNATURE_DESC* DESC)
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig;

		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		HRESULT hr = D3D12SerializeRootSignature(
			DESC,
			D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(),
			errorBlob.GetAddressOf());


		hr = device->CreateRootSignature(
			0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(rootSig.GetAddressOf()));

		return rootSig;
	}
	
}




