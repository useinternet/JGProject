#pragma once
#include "JGCore.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include "d3dx12.h"
#include "dxcapi.h"
#include "d3d12shader.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"Dxgi.lib")
#pragma comment(lib,"dxguid.lib")
//dxcapi

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif
#if defined(far)
#undef far
#endif

#if defined(near)
#undef near
#endif


using namespace Microsoft::WRL;



namespace JG
{
	ComPtr<IDXGIFactory4>   CreateDXGIFactory();
	
	ComPtr<ID3D12Device5>    CreateD3DDevice(ComPtr<IDXGIFactory4> factory, bool is_UseWrapDevice, DXGI_ADAPTER_DESC1* OutadapterDesc, bool* IsSupportedRayTracing);
	
	ComPtr<IDXGISwapChain4> CreateDXGISwapChain(
		HWND hWnd, ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12CommandQueue> cmdQue,
		DXGI_FORMAT format, u32 width, u32 height, u32 bufferCount);

	
	// D3D Command
	ComPtr<ID3D12CommandQueue>        CreateD3DCommandQueue(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);
	ComPtr<ID3D12GraphicsCommandList4> CreateD3DCommandList(ComPtr<ID3D12Device> device, ComPtr<ID3D12CommandAllocator> cmdAllocator, D3D12_COMMAND_LIST_TYPE type);
	ComPtr<ID3D12CommandAllocator>	  CreateD3DCommandAllocator(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);


	// D3D Coms
	ComPtr<ID3D12DescriptorHeap> CreateD3DDescriptorHeap(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag, u32 numDescriptor);
	ComPtr<ID3D12RootSignature>  CreateD3DRootSignature(ComPtr<ID3D12Device> device,  CD3DX12_ROOT_SIGNATURE_DESC* DESC);

	IDxcBlob* CompileShader(const String& filePath, const String& sourceCode, const String& entry, const String& target);
	inline bool IsDirectXRaytracingSupported(IDXGIAdapter1* adapter)
	{
		ComPtr<ID3D12Device> testDevice;
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};

		return SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&testDevice)))
			&& SUCCEEDED(testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData, sizeof(featureSupportData)))
			&& featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
	}

#ifdef _M_X64
#define ENABLE_SSE_CRC32 1
#else
#define ENABLE_SSE_CRC32 0
#endif

#if ENABLE_SSE_CRC32
#pragma intrinsic(_mm_crc32_u32)
#pragma intrinsic(_mm_crc32_u64)
#endif
	inline u64 HashRange(const u32* const Begin, const u32* const End, u64 Hash)
	{
#if ENABLE_SSE_CRC32
		const u64* Iter64 = (const u64*)(((u64)Begin + 7) & ~7);
		const u64* const End64 = (const u64* const)((u64)End & ~7);

		// If not 64-bit aligned, start with a single u32
		if ((u32*)Iter64 > Begin)
			Hash = _mm_crc32_u32((u32)Hash, *Begin);

		// Iterate over consecutive u64 values
		while (Iter64 < End64)
			Hash = _mm_crc32_u64((u64)Hash, *Iter64++);

		// If there is a 32-bit remainder, accumulate that
		if ((u32*)Iter64 < End)
			Hash = _mm_crc32_u32((u32)Hash, *(u32*)Iter64);
#else
		// An inexpensive hash for CPUs lacking SSE4.2
		for (const u32* Iter = Begin; Iter < End; ++Iter)
			Hash = 16777619U * Hash ^ *Iter;
#endif

		return Hash;
	}

	template <typename T>
	inline u64 HashState(const T* StateDesc, u64 Count = 1, u64 Hash = 2166136261U)
	{
		static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");
		return HashRange((u32*)StateDesc, (u32*)(StateDesc + Count), Hash);
	}
}


namespace std
{
	template <typename T>
	inline void dx_12_hash_combine(std::size_t& seed, const T& v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}


	template<>
	struct hash<D3D12_RENDER_TARGET_VIEW_DESC>
	{
		size_t operator()(const D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc) const noexcept
		{
			size_t seed = 0;

			dx_12_hash_combine(seed, rtvDesc.Format);
			dx_12_hash_combine(seed, rtvDesc.ViewDimension);

			switch (rtvDesc.ViewDimension)
			{
			case D3D12_RTV_DIMENSION_BUFFER:
				dx_12_hash_combine(seed, rtvDesc.Buffer.FirstElement);
				dx_12_hash_combine(seed, rtvDesc.Buffer.NumElements);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE1D:
				dx_12_hash_combine(seed, rtvDesc.Texture1D.MipSlice);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
				dx_12_hash_combine(seed, rtvDesc.Texture1DArray.ArraySize);
				dx_12_hash_combine(seed, rtvDesc.Texture1DArray.FirstArraySlice);
				dx_12_hash_combine(seed, rtvDesc.Texture1DArray.MipSlice);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2D:
				dx_12_hash_combine(seed, rtvDesc.Texture2D.MipSlice);
				dx_12_hash_combine(seed, rtvDesc.Texture2D.PlaneSlice);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
				dx_12_hash_combine(seed, rtvDesc.Texture2DArray.ArraySize);
				dx_12_hash_combine(seed, rtvDesc.Texture2DArray.FirstArraySlice);
				dx_12_hash_combine(seed, rtvDesc.Texture2DArray.MipSlice);
				dx_12_hash_combine(seed, rtvDesc.Texture2DArray.PlaneSlice);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2DMS:
				dx_12_hash_combine(seed, rtvDesc.Texture2DMS.UnusedField_NothingToDefine);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
				dx_12_hash_combine(seed, rtvDesc.Texture2DMSArray.ArraySize);
				dx_12_hash_combine(seed, rtvDesc.Texture2DMSArray.FirstArraySlice);
				break;
			case D3D12_RTV_DIMENSION_TEXTURE3D:
				dx_12_hash_combine(seed, rtvDesc.Texture3D.FirstWSlice);
				dx_12_hash_combine(seed, rtvDesc.Texture3D.MipSlice);
				dx_12_hash_combine(seed, rtvDesc.Texture3D.WSize);
				break;
			}

			return seed;
		}
	};


	template<>
	struct hash<D3D12_DEPTH_STENCIL_VIEW_DESC>
	{
		std::size_t operator()(const D3D12_DEPTH_STENCIL_VIEW_DESC& dsvDesc) const noexcept
		{
			std::size_t seed = 0;
			dx_12_hash_combine(seed, dsvDesc.Format);
			dx_12_hash_combine(seed, dsvDesc.ViewDimension);
			dx_12_hash_combine(seed, dsvDesc.Flags);

			switch (dsvDesc.ViewDimension)
			{
			case D3D12_DSV_DIMENSION_TEXTURE1D:
				dx_12_hash_combine(seed, dsvDesc.Texture1D.MipSlice);
				break;
			case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
				dx_12_hash_combine(seed, dsvDesc.Texture1DArray.ArraySize);
				dx_12_hash_combine(seed, dsvDesc.Texture1DArray.FirstArraySlice);
				dx_12_hash_combine(seed, dsvDesc.Texture1DArray.MipSlice);
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2D:
				dx_12_hash_combine(seed, dsvDesc.Texture2D.MipSlice);
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
				dx_12_hash_combine(seed, dsvDesc.Texture2DArray.ArraySize);
				dx_12_hash_combine(seed, dsvDesc.Texture2DArray.FirstArraySlice);
				dx_12_hash_combine(seed, dsvDesc.Texture2DArray.MipSlice);
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2DMS:
				dx_12_hash_combine(seed, dsvDesc.Texture2DMS.UnusedField_NothingToDefine);
				break;
			case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
				dx_12_hash_combine(seed, dsvDesc.Texture2DMSArray.ArraySize);
				dx_12_hash_combine(seed, dsvDesc.Texture2DMSArray.FirstArraySlice);
				break;

			}
			return seed;
		}
	};

	template<>
	struct hash<D3D12_SHADER_RESOURCE_VIEW_DESC>
	{
		std::size_t operator()(const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc) const noexcept
		{
			std::size_t seed = 0;

			dx_12_hash_combine(seed, srvDesc.Format);
			dx_12_hash_combine(seed, srvDesc.ViewDimension);
			dx_12_hash_combine(seed, srvDesc.Shader4ComponentMapping);

			switch (srvDesc.ViewDimension)
			{
			case D3D12_SRV_DIMENSION_BUFFER:
				dx_12_hash_combine(seed, srvDesc.Buffer.FirstElement);
				dx_12_hash_combine(seed, srvDesc.Buffer.NumElements);
				dx_12_hash_combine(seed, srvDesc.Buffer.StructureByteStride);
				dx_12_hash_combine(seed, srvDesc.Buffer.Flags);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE1D:
				dx_12_hash_combine(seed, srvDesc.Texture1D.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.Texture1D.MipLevels);
				dx_12_hash_combine(seed, srvDesc.Texture1D.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
				dx_12_hash_combine(seed, srvDesc.Texture1DArray.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.Texture1DArray.MipLevels);
				dx_12_hash_combine(seed, srvDesc.Texture1DArray.FirstArraySlice);
				dx_12_hash_combine(seed, srvDesc.Texture1DArray.ArraySize);
				dx_12_hash_combine(seed, srvDesc.Texture1DArray.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2D:
				dx_12_hash_combine(seed, srvDesc.Texture2D.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.Texture2D.MipLevels);
				dx_12_hash_combine(seed, srvDesc.Texture2D.PlaneSlice);
				dx_12_hash_combine(seed, srvDesc.Texture2D.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.MipLevels);
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.FirstArraySlice);
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.ArraySize);
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.PlaneSlice);
				dx_12_hash_combine(seed, srvDesc.Texture2DArray.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DMS:
				dx_12_hash_combine(seed, srvDesc.Texture2DMS.UnusedField_NothingToDefine);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
				dx_12_hash_combine(seed, srvDesc.Texture2DMSArray.FirstArraySlice);
				dx_12_hash_combine(seed, srvDesc.Texture2DMSArray.ArraySize);
				break;
			case D3D12_SRV_DIMENSION_TEXTURE3D:
				dx_12_hash_combine(seed, srvDesc.Texture3D.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.Texture3D.MipLevels);
				dx_12_hash_combine(seed, srvDesc.Texture3D.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURECUBE:
				dx_12_hash_combine(seed, srvDesc.TextureCube.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.TextureCube.MipLevels);
				dx_12_hash_combine(seed, srvDesc.TextureCube.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
				dx_12_hash_combine(seed, srvDesc.TextureCubeArray.MostDetailedMip);
				dx_12_hash_combine(seed, srvDesc.TextureCubeArray.MipLevels);
				dx_12_hash_combine(seed, srvDesc.TextureCubeArray.First2DArrayFace);
				dx_12_hash_combine(seed, srvDesc.TextureCubeArray.NumCubes);
				dx_12_hash_combine(seed, srvDesc.TextureCubeArray.ResourceMinLODClamp);
				break;
			case D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE:
				dx_12_hash_combine(seed, srvDesc.RaytracingAccelerationStructure.Location);
				break;
			}

			return seed;
		}
	};

	template<>
	struct hash<D3D12_UNORDERED_ACCESS_VIEW_DESC>
	{
		std::size_t operator()(const D3D12_UNORDERED_ACCESS_VIEW_DESC& uavDesc) const noexcept
		{
			std::size_t seed = 0;

			dx_12_hash_combine(seed, uavDesc.Format);
			dx_12_hash_combine(seed, uavDesc.ViewDimension);

			switch (uavDesc.ViewDimension)
			{
			case D3D12_UAV_DIMENSION_BUFFER:
				dx_12_hash_combine(seed, uavDesc.Buffer.FirstElement);
				dx_12_hash_combine(seed, uavDesc.Buffer.NumElements);
				dx_12_hash_combine(seed, uavDesc.Buffer.StructureByteStride);
				dx_12_hash_combine(seed, uavDesc.Buffer.CounterOffsetInBytes);
				dx_12_hash_combine(seed, uavDesc.Buffer.Flags);
				break;
			case D3D12_UAV_DIMENSION_TEXTURE1D:
				dx_12_hash_combine(seed, uavDesc.Texture1D.MipSlice);
				break;
			case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
				dx_12_hash_combine(seed, uavDesc.Texture1DArray.MipSlice);
				dx_12_hash_combine(seed, uavDesc.Texture1DArray.FirstArraySlice);
				dx_12_hash_combine(seed, uavDesc.Texture1DArray.ArraySize);
				break;
			case D3D12_UAV_DIMENSION_TEXTURE2D:
				dx_12_hash_combine(seed, uavDesc.Texture2D.MipSlice);
				dx_12_hash_combine(seed, uavDesc.Texture2D.PlaneSlice);
				break;
			case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
				dx_12_hash_combine(seed, uavDesc.Texture2DArray.MipSlice);
				dx_12_hash_combine(seed, uavDesc.Texture2DArray.FirstArraySlice);
				dx_12_hash_combine(seed, uavDesc.Texture2DArray.ArraySize);
				dx_12_hash_combine(seed, uavDesc.Texture2DArray.PlaneSlice);
				break;
			case D3D12_UAV_DIMENSION_TEXTURE3D:
				dx_12_hash_combine(seed, uavDesc.Texture3D.MipSlice);
				dx_12_hash_combine(seed, uavDesc.Texture3D.FirstWSlice);
				dx_12_hash_combine(seed, uavDesc.Texture3D.WSize);
				break;
			}

			return seed;
		}




	};
}