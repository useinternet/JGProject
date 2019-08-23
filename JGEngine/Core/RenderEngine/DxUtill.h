#pragma once
#include "RE.h"

using namespace Microsoft::WRL;
namespace RE
{
	ComPtr<IDXGIFactory4> CreateDXGIFactory();


	ComPtr<ID3D12Device> CreateD3DDevice(
		ComPtr<IDXGIFactory4> factory,
		bool is_UseWrapDevice,
		DXGI_ADAPTER_DESC1* OutadapterDesc = nullptr);


	ComPtr<IDXGISwapChain4> CreateDXGISwapChain(
		HWND hWnd,
		ComPtr<IDXGIFactory4> factory,
		ComPtr<ID3D12CommandQueue> cmdQue,
		DXGI_FORMAT format,
		uint32_t width,
		uint32_t height,
		uint32_t bufferCount);



	ComPtr<ID3D12CommandQueue> CreateD3DCommandQueue(
		ComPtr<ID3D12Device> device,
		D3D12_COMMAND_LIST_TYPE type);


	ComPtr<ID3D12GraphicsCommandList> CreateD3DCommandList(
		ComPtr<ID3D12Device> device,
		ComPtr<ID3D12CommandAllocator> cmdAllocator,
		D3D12_COMMAND_LIST_TYPE type);


	ComPtr<ID3D12CommandAllocator> CreateD3DCommandAllocator(
		ComPtr<ID3D12Device> device,
		D3D12_COMMAND_LIST_TYPE type);


	ComPtr<ID3D12DescriptorHeap> CreateD3DDescriptorHeap(
		ComPtr<ID3D12Device> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		D3D12_DESCRIPTOR_HEAP_FLAGS flag,
		uint32_t numDescriptor);

	ComPtr<ID3D12RootSignature> CreateD3DRootSignature(ComPtr<ID3D12Device> device, CD3DX12_ROOT_SIGNATURE_DESC* DESC);


	bool CheckTearingSupport(ComPtr<IDXGIFactory4> factory);
}