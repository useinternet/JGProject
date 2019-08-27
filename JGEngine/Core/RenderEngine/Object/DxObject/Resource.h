#pragma once
#include "DxObject.h"
#include "DescriptorHandle.h"

namespace RE
{
	class ResourceDataMap;
	class _ResourceData;
	class RENDERENGINE_API Resource : public DxObject
	{
	public:
		Resource();
		Resource(const std::string& name);
		Resource(const Resource& copy);
		Resource(Resource&& rhs);
		Resource& operator=(const Resource& rhs);
		Resource& operator=(Resource&& rhs);
		virtual ~Resource() override;
	public:
		void CreateResource(
			const D3D12_RESOURCE_DESC& desc,
			D3D12_CLEAR_VALUE* clear_value = nullptr,
			D3D12_RESOURCE_STATES init_state = D3D12_RESOURCE_STATE_COMMON);

		void CreateResource(
			const D3D12_HEAP_PROPERTIES& heap_properties,
			D3D12_HEAP_FLAGS heap_flags,
			const D3D12_RESOURCE_DESC& desc,
			D3D12_RESOURCE_STATES init_state,
			D3D12_CLEAR_VALUE* clear_value = nullptr);
		void SetD3DResource(Microsoft::WRL::ComPtr<ID3D12Resource> d3d_resource, D3D12_RESOURCE_STATES init_State);
		void Resize(uint64_t width, uint32_t height);
		void Reset();


		bool IsVaild() const;
		const _ResourceData* GetData() const;
		D3D12_RESOURCE_DESC GetDesc() const;
		uint32_t GetRefCount() const;

		const Color& GetClearColor() const;
		float GetClearDepth() const;
		uint8_t GetClearStencil() const;
		ID3D12Resource* GetD3DResource() const;
		virtual void SetName(const std::string& name) override;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_D3D_Resource;
		Color m_ClearColor = Color();
		float m_ClearDepth = 0.0f;
		uint8_t m_ClearStencil = 0;
	};

	class RENDERENGINE_API Texture : public Resource
	{
	public:
		Texture();
		Texture(const std::string& name);

		D3D12_CPU_DESCRIPTOR_HANDLE GetSrv() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetRtv() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDsv() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUav() const;

		void SetSrvDesc(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);
		void SetSrvDescNull();


		void SetRtvDesc(const D3D12_RENDER_TARGET_VIEW_DESC& desc);
		void SetRtvDescNull();


		void SetDsvDesc(const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);
		void SetDsvDescNull();

		void SetUavDesc(const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);
		void SetUavDescNull();
	private:
		std::shared_ptr<D3D12_SHADER_RESOURCE_VIEW_DESC>    m_SrvDesc;
		std::shared_ptr<D3D12_RENDER_TARGET_VIEW_DESC>      m_RtvDesc;
		std::shared_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC>      m_DsvDesc;
		std::shared_ptr<D3D12_UNORDERED_ACCESS_VIEW_DESC>   m_UavDesc;
	};

	class Buffer : public Resource
	{
	public:
		Buffer();
		Buffer(const std::string& name);

		virtual void CreateView(uint32_t numElements, uint32_t elementSize) = 0;
	};

	class VertexBuffer : public Buffer
	{
	public:
		VertexBuffer();
		VertexBuffer(const std::string& name);
		virtual void CreateView(uint32_t numVertices, uint32_t vertexStride) override;
		D3D12_VERTEX_BUFFER_VIEW GetView() const {
			return m_VertexBufferView;
		}
	private:
		uint32_t m_NumVertices;
		uint32_t m_VertexStride;

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	};

	class IndexBuffer : public Buffer
	{
	public:
		IndexBuffer();
		IndexBuffer(const std::string& name);
		virtual void CreateView(uint32_t numIndicies, uint32_t indexSize) override;
		D3D12_INDEX_BUFFER_VIEW GetView() const {
			return m_IndexBufferView;
		}
	private:
		uint32_t    m_NumIndicies;
		DXGI_FORMAT m_IndexFormat;

		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	};

	// Resource
}
