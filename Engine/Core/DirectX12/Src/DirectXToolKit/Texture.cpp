#include<PCH.h>
#include"Texture.h"
#include"DirectXCore.h"
#include"ResourceStateMap.h"
#include<Util/Hash.h>
using namespace std;
using namespace Dx12;




Texture::Texture(ETextureUsage usage, const std::string& name) :
	Resource(name), m_Usage(usage)
{
	Reset_ResourceHandle();
}
Texture::Texture(
	ETextureUsage usage,
	const D3D12_RESOURCE_DESC& desc,
	const D3D12_CLEAR_VALUE* clearValue,
	const std::string& name ,
	D3D12_RESOURCE_STATES initState,
	D3D12_HEAP_TYPE heapType ,
	D3D12_HEAP_FLAGS heapFlag ) :
	m_Usage(usage), Resource(desc, clearValue, name, initState, heapType, heapFlag)
{
	Reset_ResourceHandle();
}
Texture::Texture(const Texture& copy) :
	Resource(copy), m_Usage(copy.m_Usage)
{
	Reset_ResourceHandle();
}
Texture::Texture(Texture&& copy) :
	Resource(copy), m_Usage(copy.m_Usage)
{
	m_SRVs = move(copy.m_SRVs);
	m_UAVs = move(copy.m_UAVs);
	m_RTVs = move(copy.m_RTVs);
	m_DSVs = move(copy.m_DSVs);
}
Texture& Texture::operator=(const Texture& rhs)
{
	Resource::operator=(rhs);
	m_Usage = rhs.m_Usage;
	Reset_ResourceHandle();
	return *this;
}
Texture& Texture::operator=(Texture&& rhs)
{
	Resource::operator=(rhs);
	m_Usage = rhs.m_Usage;

	m_SRVs = move(rhs.m_SRVs);
	m_UAVs = move(rhs.m_UAVs);
	m_RTVs = move(rhs.m_RTVs);
	m_DSVs = move(rhs.m_DSVs);

	return *this;
}
void Texture::Resize(uint32_t width, uint32_t height, uint16_t depth_or_array_size)
{
	if (m_D3D_Resource)
	{
		CD3DX12_RESOURCE_DESC desc(m_D3D_Resource->GetDesc());

		desc.Width = std::max<uint32_t>(width, 1u);
		desc.Height = std::max<uint32_t>(height, 1u);
		desc.DepthOrArraySize = depth_or_array_size;

		auto device = DxDevice::GetDevice();

		ResourceStateMap::RemoveResourceState(m_D3D_Resource.Get());


		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			m_ClearValue.get(),
			IID_PPV_ARGS(&m_D3D_Resource)));

		ResourceStateMap::AddResourceState(m_D3D_Resource.Get(), D3D12_RESOURCE_STATE_COMMON);

		SetName(m_Name);
		Reset_ResourceHandle();
	}
}
void Texture::Reset_ResourceHandle()
{
	m_SRVs.clear();
	m_UAVs.clear();
	m_RTVs.clear();
	m_DSVs.clear();
}
D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* Desc) const
{
	uint32_t hashCode = 0;
	if (Desc)
	{
		hashCode = (uint32_t)Common::Util::HashState<D3D12_SHADER_RESOURCE_VIEW_DESC>(Desc);
	}



	auto iter = m_SRVs.find(hashCode);
	if (iter != m_SRVs.end())
	{
		return iter->second.GetDescriptorHandle();
	}
	else
	{
		auto device = DxDevice::GetDevice();
		DescriptorAllocation alloc = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CreateShaderResourceView(m_D3D_Resource.Get(), Desc, alloc.GetDescriptorHandle());

		m_SRVs[hashCode] = move(alloc);

		return m_SRVs[hashCode].GetDescriptorHandle();
	}
}
D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc) const
{
	if (!(GetDesc().Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS))
	{
		DX12_LOG_ERROR("%s is not allow unorderedaccess. ", GetName().c_str());
		return { 0 };
	}
	uint32_t hashCode = 0;
	if (Desc)
	{
		hashCode = (uint32_t)Common::Util::HashState<D3D12_UNORDERED_ACCESS_VIEW_DESC>(Desc);
	}

	auto iter = m_UAVs.find(hashCode);
	if (iter != m_UAVs.end())
	{
		return iter->second.GetDescriptorHandle();
	}
	else
	{
		auto device = DxDevice::GetDevice();
		DescriptorAllocation alloc = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CreateUnorderedAccessView(m_D3D_Resource.Get(), nullptr, Desc, alloc.GetDescriptorHandle());

		m_UAVs[hashCode] = move(alloc);

		return m_UAVs[hashCode].GetDescriptorHandle();
	}

}
D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetRenderTargetView(const D3D12_RENDER_TARGET_VIEW_DESC* Desc) const
{
	if (!(GetDesc().Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) )
	{
		DX12_LOG_ERROR("%s is not allow rendertarget. ", GetName().c_str());
		return { 0 };
	}
	uint32_t hashCode = 0;
	if (Desc)
	{
		hashCode = (uint32_t)Common::Util::HashState<D3D12_RENDER_TARGET_VIEW_DESC>(Desc);
	}

	auto iter = m_RTVs.find(hashCode);
	if (iter != m_RTVs.end())
	{
		return iter->second.GetDescriptorHandle();
	}
	else
	{
		auto device = DxDevice::GetDevice();
		DescriptorAllocation alloc = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		device->CreateRenderTargetView(m_D3D_Resource.Get(), Desc, alloc.GetDescriptorHandle());
		m_RTVs[hashCode] = move(alloc);

		return m_RTVs[hashCode].GetDescriptorHandle();
	}
}
D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetDepthStencilView(const D3D12_DEPTH_STENCIL_VIEW_DESC* Desc) const
{
	if ( !(GetDesc().Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL))
	{
		DX12_LOG_ERROR("%s is not allow depthstencil. ", GetName().c_str());
		return { 0 };
	}
	uint32_t hashCode = 0;
	if (Desc)
	{
		hashCode = (uint32_t)Common::Util::HashState<D3D12_DEPTH_STENCIL_VIEW_DESC>(Desc);

	}

	auto iter = m_DSVs.find(hashCode);
	if (iter != m_DSVs.end())
	{
		return iter->second.GetDescriptorHandle();
	}
	else
	{
		auto device = DxDevice::GetDevice();
		DescriptorAllocation alloc = DxDevice::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		device->CreateDepthStencilView(m_D3D_Resource.Get(), Desc, alloc.GetDescriptorHandle());
		m_DSVs[hashCode] = move(alloc);

		return m_DSVs[hashCode].GetDescriptorHandle();
	}
}


bool Texture::IsUAVCompatibleFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SINT:
		return true;
	default:
		return false;
	}
}
bool Texture::IsSRGBFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return true;
	default:
		return false;
	}
}
bool Texture::IsBGRFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return true;
	default:
		return false;
	}
}
bool Texture::IsDepthFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_D16_UNORM:
		return true;
	default:
		return false;
	}
}
DXGI_FORMAT Texture::GetTypelessFormat(DXGI_FORMAT format)
{
	DXGI_FORMAT typelessFormat = format;

	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		typelessFormat = DXGI_FORMAT_R32G32B32A32_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		typelessFormat = DXGI_FORMAT_R32G32B32_TYPELESS;
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
		typelessFormat = DXGI_FORMAT_R16G16B16A16_TYPELESS;
		break;
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
		typelessFormat = DXGI_FORMAT_R32G32_TYPELESS;
		break;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		typelessFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
		break;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
		typelessFormat = DXGI_FORMAT_R10G10B10A2_TYPELESS;
		break;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		typelessFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		break;
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
		typelessFormat = DXGI_FORMAT_R16G16_TYPELESS;
		break;
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
		typelessFormat = DXGI_FORMAT_R32_TYPELESS;
		break;
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
		typelessFormat = DXGI_FORMAT_R8G8_TYPELESS;
		break;
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
		typelessFormat = DXGI_FORMAT_R16_TYPELESS;
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
		typelessFormat = DXGI_FORMAT_R8_TYPELESS;
		break;
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		typelessFormat = DXGI_FORMAT_BC1_TYPELESS;
		break;
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		typelessFormat = DXGI_FORMAT_BC2_TYPELESS;
		break;
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		typelessFormat = DXGI_FORMAT_BC3_TYPELESS;
		break;
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		typelessFormat = DXGI_FORMAT_BC4_TYPELESS;
		break;
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		typelessFormat = DXGI_FORMAT_BC5_TYPELESS;
		break;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		typelessFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
		break;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		typelessFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
		break;
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
		typelessFormat = DXGI_FORMAT_BC6H_TYPELESS;
		break;
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		typelessFormat = DXGI_FORMAT_BC7_TYPELESS;
		break;
	}

	return typelessFormat;
}