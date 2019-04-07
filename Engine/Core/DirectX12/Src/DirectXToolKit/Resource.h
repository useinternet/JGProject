#pragma once

#include"DirectXCommon/DirectXCommon.h"

namespace Dx12
{
	class CommandList;
	class Resource
	{
	protected:
		std::string m_Name;
		ComPtr<ID3D12Resource>             m_D3D_Resource;
		std::unique_ptr<D3D12_CLEAR_VALUE> m_ClearValue;
	public:
		Resource(const std::string& name = "ID3D12Resource");


		Resource(
			const D3D12_RESOURCE_DESC& desc, 
			const D3D12_CLEAR_VALUE* clearValue = nullptr,
			const std::string& name             = "ID3D12Resource",
			D3D12_RESOURCE_STATES initState     = D3D12_RESOURCE_STATE_COMMON,
			D3D12_HEAP_TYPE heapType            = D3D12_HEAP_TYPE_DEFAULT,
			D3D12_HEAP_FLAGS heapFlag           = D3D12_HEAP_FLAG_NONE);

		Resource(const Resource& copy);
		Resource(Resource&& rhs);

		Resource& operator=(const Resource& rhs);
		Resource& operator=(Resource&& rhs);

		virtual ~Resource();

	public:
		void Reset();
		void Set(const Resource& resource);
		void SetName(const std::string& name);

		const std::string& GetName() const {
			return m_Name;
		}
		void SetD3DResource(
			ComPtr<ID3D12Resource> d3dResource,
			D3D12_RESOURCE_STATES initState,
			D3D12_CLEAR_VALUE* clearValue = nullptr);

		bool IsValid() const {
			return (m_D3D_Resource != nullptr);
		}

		ID3D12Resource* Get() const {
			return m_D3D_Resource.Get();
		}
		D3D12_RESOURCE_DESC GetDesc() const {
			D3D12_RESOURCE_DESC desc = {};
			if (m_D3D_Resource)
			{
				desc = m_D3D_Resource->GetDesc();
			}
			return desc;
		}
		D3D12_RESOURCE_STATES GetState() const;
		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* Desc = nullptr)   const { return { 0 }; };
		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc = nullptr) const { return { 0 }; };
	};
}