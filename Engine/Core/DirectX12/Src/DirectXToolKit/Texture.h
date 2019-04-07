#pragma once
#include"Resource.h"
#include"DescriptorAllocation.h"
namespace Dx12
{
	class Texture : public Resource
	{

	private:
		ETextureUsage m_Usage;
		mutable std::unordered_map<uint32_t, DescriptorAllocation> m_SRVs;
		mutable std::unordered_map<uint32_t, DescriptorAllocation> m_UAVs;
		mutable std::unordered_map<uint32_t, DescriptorAllocation> m_RTVs;
		mutable std::unordered_map<uint32_t, DescriptorAllocation> m_DSVs;
	public:
		Texture() : Resource("Texture") {}
		Texture(ETextureUsage usage, const std::string& name = "Texture");
		Texture(
			ETextureUsage usage,
			const D3D12_RESOURCE_DESC& desc,
			const D3D12_CLEAR_VALUE* clearValue = nullptr,
			const std::string& name = "Texture",
			D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON,
			D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT,
			D3D12_HEAP_FLAGS heapFlag = D3D12_HEAP_FLAG_NONE);
		Texture(const Texture& copy);
		Texture(Texture&& copy);
		Texture& operator=(const Texture& rhs);
		Texture& operator=(Texture&& rhs);
		virtual ~Texture() = default;
	public:
		void Resize(uint32_t width, uint32_t height, uint16_t depth_or_array_size = 1);
		void Reset_ResourceHandle();
	public:
		ETextureUsage GetUsage() const {
			return m_Usage;
		}
		void SetUsage(ETextureUsage usage) {
			m_Usage = usage;
		}
	public:
		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* Desc = nullptr) const;
		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc = nullptr) const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView(const D3D12_RENDER_TARGET_VIEW_DESC* Desc = nullptr) const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView(const D3D12_DEPTH_STENCIL_VIEW_DESC* Desc = nullptr) const;


	public:
		static bool IsUAVCompatibleFormat(DXGI_FORMAT format);
		static bool IsSRGBFormat(DXGI_FORMAT format);
		static bool IsBGRFormat(DXGI_FORMAT format);
		static bool IsDepthFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetTypelessFormat(DXGI_FORMAT format);
	};
}