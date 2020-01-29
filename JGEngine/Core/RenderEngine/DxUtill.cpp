#include "pch.h"
#include "DxUtill.h"

#ifdef _DEBUG
#	include "Initguid.h"
#	include "DXGIDebug.h"
#endif // _DEBUG
#include "d3dx12.h"
using namespace std;

namespace RE
{
	ComPtr<IDXGIFactory4> CreateDXGIFactory()
	{
		HRESULT hr = S_OK;
		ComPtr<IDXGIFactory4> factory;
		UINT32 dxgiFactoryFlags = 0;

#ifdef _DEBUG
		ComPtr<ID3D12Debug> debugInterface;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
		assert(SUCCEEDED(hr) && "failed DebugInterface");

		debugInterface->EnableDebugLayer();

		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
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
	ComPtr<ID3D12Device>  CreateD3DDevice(
		ComPtr<IDXGIFactory4> factory,
		bool is_UseWrapDevice,
		DXGI_ADAPTER_DESC1* OutadapterDesc)
	{
		HRESULT hr = S_OK;
		if (is_UseWrapDevice)
		{
			ComPtr<IDXGIAdapter>  warpAdapter;
			ComPtr<ID3D12Device>  pDevice;
			hr = factory->EnumWarpAdapter(IID_PPV_ARGS(warpAdapter.GetAddressOf()));
			assert(SUCCEEDED(hr) && "failed Create warpAdapter");

			hr = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(pDevice.GetAddressOf()));
			assert(SUCCEEDED(hr) && "failed Create D3D12Device");
			return pDevice;
		}
		else
		{
			ComPtr<IDXGIAdapter1> pAdapter;
			ComPtr<ID3D12Device>  pDevice;

			DXGI_ADAPTER_DESC1 adapterDesc = {};
			SIZE_T maxSize = 0;

			for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(Idx, &pAdapter); ++Idx)
			{
				DXGI_ADAPTER_DESC1 desc;
				pAdapter->GetDesc1(&desc);
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				if (desc.DedicatedVideoMemory > maxSize &&
					SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
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

			return pDevice;
		}
	}
	ComPtr<IDXGISwapChain4> CreateDXGISwapChain(
		HWND hWnd,
		ComPtr<IDXGIFactory4> factory,
		ComPtr<ID3D12CommandQueue> cmdQue,
		DXGI_FORMAT format,
		uint32_t width,
		uint32_t height,
		uint32_t bufferCount)
	{
		HRESULT hr = S_OK;
		ComPtr<IDXGISwapChain4> dxgiSwapChain4;

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
		swapChainDesc.Flags = CheckTearingSupport(factory) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		ComPtr<IDXGISwapChain1> swapChain1;
		hr = factory->CreateSwapChainForHwnd(cmdQue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
		assert(SUCCEEDED(hr) && "failed Create IDXGISwapChain4");
		hr = factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
		assert(SUCCEEDED(hr) && "failed MakeWindowAssociation");
		hr = swapChain1.As(&dxgiSwapChain4);
		assert(SUCCEEDED(hr) && "failed SwapChain1 As SwapChain4");
		return dxgiSwapChain4;
	}



	ComPtr<ID3D12CommandQueue> CreateD3DCommandQueue(
		ComPtr<ID3D12Device> device,
		D3D12_COMMAND_LIST_TYPE type)
	{
		HRESULT hr = S_OK;
		ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue));
		assert(SUCCEEDED(hr) && "failed Create ID3D12CommandQueue");
		return d3d12CommandQueue;

	}


	ComPtr<ID3D12GraphicsCommandList> CreateD3DCommandList(
		ComPtr<ID3D12Device> device,
		ComPtr<ID3D12CommandAllocator> cmdAllocator,
		D3D12_COMMAND_LIST_TYPE type)
	{

		HRESULT hr = S_OK;
		ComPtr<ID3D12GraphicsCommandList> commandList;
		hr = device->CreateCommandList(0, type, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
		assert(SUCCEEDED(hr) && "failed Create ID3D12GraphicsCommandList");
		return commandList;
	}


	ComPtr<ID3D12CommandAllocator> CreateD3DCommandAllocator(
		ComPtr<ID3D12Device> device,
		D3D12_COMMAND_LIST_TYPE type)
	{
		HRESULT hr = S_OK;
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));
		

		assert(SUCCEEDED(hr) && "failed Create ID3D12CommandAllocator");
		return commandAllocator;
	}



	ComPtr<ID3D12DescriptorHeap> CreateD3DDescriptorHeap(
		ComPtr<ID3D12Device> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		D3D12_DESCRIPTOR_HEAP_FLAGS flag,
		uint32_t numDescriptor)
	{
		HRESULT hr = S_OK;
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = numDescriptor;
		desc.Type = type;
		desc.Flags = flag;

		hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
		hr = device->GetDeviceRemovedReason();
		assert(SUCCEEDED(hr) && "failed Create ID3D12DescriptorHeap");


		return descriptorHeap;

	}
	ComPtr<ID3D12RootSignature> CreateD3DRootSignature(ComPtr<ID3D12Device> device, CD3DX12_ROOT_SIGNATURE_DESC* DESC)
	{
		ComPtr<ID3D12RootSignature> rootSig;

		ComPtr<ID3DBlob> serializedRootSig;
		ComPtr<ID3DBlob> errorBlob;

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

	bool CheckTearingSupport(ComPtr<IDXGIFactory4> factory)
	{
		return false;
		bool allowTearing = false;

		ComPtr<IDXGIFactory5> factory5;
		if (SUCCEEDED(factory.As(&factory5)))
		{
			if (FAILED(factory5->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING,
				&allowTearing, sizeof(allowTearing))))
			{
				allowTearing = false;
			}
		}

		return allowTearing;
	}

}