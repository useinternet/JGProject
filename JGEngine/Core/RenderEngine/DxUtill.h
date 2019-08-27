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


	// ToString
	inline std::string Dx12ToString(D3D12_RESOURCE_DIMENSION dimension)
	{
		switch (dimension)
		{
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER:
			return "Buffer";
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			return "Texture1D";
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			return "Texture2D";
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			return "Texture3D";
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN:
			return "Unknown";
		}
		return "None";
	}
	inline std::string Dx12ToString(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_UNKNOWN:
			return "Unknown";
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			return "R32G32B32A32 Typeless";
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return "R32G32B32A32 Float";
		case DXGI_FORMAT_R32G32B32A32_UINT:
			return "R32G32B32A32 UInt";
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return "R32G32B32A32 SInt";
		case DXGI_FORMAT_R32G32B32_TYPELESS:
			return "R32G32B32 Typeless";
		case DXGI_FORMAT_R32G32B32_FLOAT:
			return "R32G32B32 Float";
		case DXGI_FORMAT_R32G32B32_UINT:
			return "R32G32B32 UInt";
		case DXGI_FORMAT_R32G32B32_SINT:
			return "R32G32B32 SInt";
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			return "R16G16B16A16 Typeless";
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return "R16G16B16A16 Float";
		case DXGI_FORMAT_R16G16B16A16_UNORM:
			return "R16G16B16A16 UNorm";
		case DXGI_FORMAT_R16G16B16A16_UINT:
			return "R16G16B16A16 UInt";
		case DXGI_FORMAT_R16G16B16A16_SNORM:
			return "R16G16B16A16 SNorm";
		case DXGI_FORMAT_R16G16B16A16_SINT:
			return "R16G16B16A16 SInt";
		case DXGI_FORMAT_R32G32_TYPELESS:
			return "R32G32 Typeless";
		case DXGI_FORMAT_R32G32_FLOAT:
			return "R32G32 Float";
		case DXGI_FORMAT_R32G32_UINT:
			return "R32G32 UInt";
		case DXGI_FORMAT_R32G32_SINT:
			return "R32G32 SInt";
		case DXGI_FORMAT_R32G8X24_TYPELESS:
			return "R32G8X24 Typeless";
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			return "D32 Float S8X24 UInt";
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			return "R32 Float S8X24 Typeless";
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return "X32 Typeless G8X24 UInt";
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			return "R10G10B10A2 TypeLess";
		case DXGI_FORMAT_R10G10B10A2_UNORM:
			return "R10G10B10A2 Unorm";
		case DXGI_FORMAT_R10G10B10A2_UINT:
			return "R10G10B10A2 UInt";
		case DXGI_FORMAT_R11G11B10_FLOAT:
			return "R11G11B10 Float";
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			return "R8G8B8A8 Typeless";
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return "R8G8B8A8 UNorm";
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return "R8G8B8A8 UNorm SRGB";
		case DXGI_FORMAT_R8G8B8A8_UINT:
			return "R8G8B8A8 UInt";
		case DXGI_FORMAT_R8G8B8A8_SNORM:
			return "R8G8B8A8 SNorm";
		case DXGI_FORMAT_R8G8B8A8_SINT:
			return "R8G8B8A8 SInt";
		case DXGI_FORMAT_R16G16_TYPELESS:
			return "R16G16 Typeless";
		case DXGI_FORMAT_R16G16_FLOAT:
			return "R16G16 Float";
		case DXGI_FORMAT_R16G16_UNORM:
			return "R16G16 UNorm";
		case DXGI_FORMAT_R16G16_UINT:
			return "R16G16 UInt";
		case DXGI_FORMAT_R16G16_SNORM:
			return "R16G16 SNorm";
		case DXGI_FORMAT_R16G16_SINT:
			return "R16G16 SInt";
		case DXGI_FORMAT_R32_TYPELESS:
			return "R32 Typeless";
		case DXGI_FORMAT_D32_FLOAT:
			return "D32 Float";
		case DXGI_FORMAT_R32_FLOAT:
			return "R32 Float";
		case DXGI_FORMAT_R32_UINT:
			return "R32 UInt";
		case DXGI_FORMAT_R32_SINT:
			return "R32 SInt";
		case DXGI_FORMAT_R24G8_TYPELESS:
			return "R24G8 Typeless";
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			return "D24 UNorm S8 UInt";
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			return "D24 UNorm X8 Typeless";
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return "X24 Typeless G8 UInt";
		case DXGI_FORMAT_R8G8_TYPELESS:
			return "R8G8 Typeless";
		case DXGI_FORMAT_R8G8_UNORM:
			return "R8G8 UNorm";
		case DXGI_FORMAT_R8G8_UINT:
			return "R8G8 UInt";
		case DXGI_FORMAT_R8G8_SNORM:
			return "R8G8 SNorm";
		case DXGI_FORMAT_R8G8_SINT:
			return "R8G8 SInt";
		case DXGI_FORMAT_R16_TYPELESS:
			return "R16 Typeless";
		case DXGI_FORMAT_R16_FLOAT:
			return "R16 Float";
		case DXGI_FORMAT_D16_UNORM:
			return "D16 UNorm";
		case DXGI_FORMAT_R16_UNORM:
			return "R16 UNorm";
		case DXGI_FORMAT_R16_UINT:
			return "R16 UInt";
		case DXGI_FORMAT_R16_SNORM:
			return "R16 SNorm";
		case DXGI_FORMAT_R16_SINT:
			return "R16 SInt";
		case DXGI_FORMAT_R8_TYPELESS:
			return "R8 Typeless";
		case DXGI_FORMAT_R8_UNORM:
			return "R8 UNorm";
		case DXGI_FORMAT_R8_UINT:
			return "R8 UInt";
		case DXGI_FORMAT_R8_SNORM:
			return "R8 SNorm";
		case DXGI_FORMAT_R8_SINT:
			return "R8 SInt";
		case DXGI_FORMAT_A8_UNORM:
			return "A8 UNorm";
		case DXGI_FORMAT_R1_UNORM:
			return "R1 UNorm";
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			return "R9G9B9E5 Sharedexp";
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
			return "R8G8B8G8 UNorm";
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
			return "G8R8 G8B8 UNorm";
		case DXGI_FORMAT_BC1_TYPELESS:
			return "BC1 Typeless";
		case DXGI_FORMAT_BC1_UNORM:
			return "BC1 UNorm";
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			return "BC1 UNorm SRGB";
		case DXGI_FORMAT_BC2_TYPELESS:
			return "BC2 Typeless";
		case DXGI_FORMAT_BC2_UNORM:
			return "BC2 UNorm";
		case DXGI_FORMAT_BC2_UNORM_SRGB:
			return "BC2 UNorm SRGB";
		case DXGI_FORMAT_BC3_TYPELESS:
			return "BC3 Typeless";
		case DXGI_FORMAT_BC3_UNORM:
			return "BC3 UNorm";
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			return "BC3 UNorm SRGB";
		case DXGI_FORMAT_BC4_TYPELESS:
			return "BC4 Typeless";
		case DXGI_FORMAT_BC4_UNORM:
			return "BC4 UNorm";
		case DXGI_FORMAT_BC4_SNORM:
			return "BC4 SNorm";
		case DXGI_FORMAT_BC5_TYPELESS:
			return "BC5 Typeless";
		case DXGI_FORMAT_BC5_UNORM:
			return "BC5 UNorm";
		case DXGI_FORMAT_BC5_SNORM:
			return "BC5 SNorm";
		case DXGI_FORMAT_B5G6R5_UNORM:
			return "B5G6R5 UNorm";
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			return "B5G5R5A1 UNorm";
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return "B8G8R8A8 UNorm";
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return "B8G8R8X8 UNorm";
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
			return "R10G10B10 Xr Bias A2 UNorm";
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			return "B8G8R8A8 Typeless";
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return "B8G8R8A8 UNorm SRGB";
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
			return "B8G8R8X8 Typeless";
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return "B8G8R8X8 UNorm SRGB";
		case DXGI_FORMAT_BC6H_TYPELESS:
			return "BC6H Typeless";
		case DXGI_FORMAT_BC6H_UF16:
			return "BC6H UF16";
		case DXGI_FORMAT_BC6H_SF16:
			return "BC6H SF16";
		case DXGI_FORMAT_BC7_TYPELESS:
			return "BC7 Typeless";
		case DXGI_FORMAT_BC7_UNORM:
			return "BC7 UNorm";
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return "BC7 UNorm SRGB";
		case DXGI_FORMAT_AYUV:
			return "AYUV";
		case DXGI_FORMAT_Y410:
			return "Y410";
		case DXGI_FORMAT_Y416:
			return "Y416";
		case DXGI_FORMAT_NV12:
			return "NV12";
		case DXGI_FORMAT_P010:
			return "P010";
		case DXGI_FORMAT_P016:
			return "P016";
		case DXGI_FORMAT_420_OPAQUE:
			return "420 Opaque";
		case DXGI_FORMAT_YUY2:
			return "YUY2";
		case DXGI_FORMAT_Y210:
			return "Y210";
		case DXGI_FORMAT_Y216:
			return "Y216";
		case DXGI_FORMAT_NV11:
			return "NV11";
		case DXGI_FORMAT_AI44:
			return "AI44";
		case DXGI_FORMAT_IA44:
			return "IA44";
		case DXGI_FORMAT_P8:
			return "P8";
		case DXGI_FORMAT_A8P8:
			return "A8P8";
		case DXGI_FORMAT_B4G4R4A4_UNORM:
			return "B4G4R4A4 UNorm";
		case DXGI_FORMAT_P208:
			return "P208";
		case DXGI_FORMAT_V208:
			return "V208";
		case DXGI_FORMAT_V408:
			return "V408";
		case DXGI_FORMAT_FORCE_UINT:
			return "Force UInt";
		}

		return "None";
	}
	inline std::string Dx12ToString(D3D12_TEXTURE_LAYOUT layout)
	{
		switch (layout)
		{
		case D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE:
			return "64KB Standard Swizzle";
		case D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE:
			return "64KB Undefined Swizzle";
		case D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_ROW_MAJOR:
			return "Row Major";
		case D3D12_TEXTURE_LAYOUT::D3D12_TEXTURE_LAYOUT_UNKNOWN:
			return "Unknown";
		}
		return "None";
	}
	inline std::string Dx12ToString(D3D12_RESOURCE_FLAGS flag)
	{
		switch (flag)
		{
		case D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER:
			return "Allow Cross Adapter";
		case D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL:
			return "Allow Depth Stencil";
		case D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET:
			return "Allow Render Target";
		case D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS:
			return "Allow Simultaneous Access";
		case D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS:
			return "Allow Unordered Access";
		case D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE:
			return "Deny Shader Reousrce";
		case D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE:
			return "None";
		case D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY:
			return "Video Decode Reference Only";
		}
		return "None";
	}
}