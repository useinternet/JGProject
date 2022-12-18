#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"
#include "DirectX12/DirectX12Define.h"

class GRAPHICS_API HDirectX12Helper
{
public:
	static HDX12ComPtr<HDX12Factory>   CreateDXGIFactory();
	static HDX12ComPtr<HDX12Device>    CreateD3DDevice(HDX12ComPtr<HDX12Factory> factory, bool bUseWrapDevice, DXGI_ADAPTER_DESC1* OutadapterDesc, bool* IsSupportedRayTracing);
	static HDX12ComPtr<HDX12SwapChain> CreateDXGISwapChain(HJWHandle handle, HDX12ComPtr<HDX12Factory> factory, HDX12ComPtr<HDX12CommandQueue> cmdQue, DXGI_FORMAT format, uint32 width, uint32 height, uint32 bufferCount);

	static HDX12ComPtr<HDX12CommandQueue>       CreateD3DCommandQueue(HDX12ComPtr<HDX12Device> device, D3D12_COMMAND_LIST_TYPE type);
	static HDX12ComPtr<HDX12CommandList>		CreateD3DCommandList(HDX12ComPtr<HDX12Device> device, HDX12ComPtr<HDX12CommandAllocator> cmdAllocator, D3D12_COMMAND_LIST_TYPE type);
	static HDX12ComPtr<HDX12CommandAllocator>	CreateD3DCommandAllocator(HDX12ComPtr<HDX12Device> device, D3D12_COMMAND_LIST_TYPE type);

	static HDX12ComPtr<HDX12DescriptorHeap> CreateD3DDescriptorHeap(HDX12ComPtr<HDX12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag, uint32 numDescriptor);
	static HDX12ComPtr<HDX12RootSignature>  CreateD3DRootSignature(HDX12ComPtr<HDX12Device> device, CD3DX12_ROOT_SIGNATURE_DESC* DESC);

	static bool IsDirectXRaytracingSupported(HDX12Adapter* adapter);

	static DXGI_FORMAT           ConvertDXGIFormat(ETextureFormat format);
	static DXGI_FORMAT           ConvertDXGIFormat(EShaderDataType type);
	static D3D12_RESOURCE_STATES ConvertDX12ResourceState(EResourceState state);

};

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