#include "pch.h"
#include "Resource.h"

#include "ResourceDataMap.h"
#include "Defines/Dx12Hash.h"
#include "RenderDevice.h"


using namespace Microsoft::WRL;
namespace RE
{

	Resource::Resource() : 
		DxObject("Resource")
	{	}
	Resource::Resource(const std::string& name) : 
		DxObject(name) { }

	Resource::Resource(const Resource& copy) :
		DxObject(copy),
		m_D3D_Resource(copy.m_D3D_Resource),
		m_ClearColor(copy.m_ClearColor),
		m_ClearDepth(copy.m_ClearDepth),
		m_ClearStencil(copy.m_ClearStencil)
	{
		ResourceDataMap::RegisterResource(m_D3D_Resource.Get(), GetName(), D3D12_RESOURCE_STATE_COMMON);
	}
	Resource::Resource(Resource&& rhs) : 
		DxObject(rhs),
		m_D3D_Resource(std::move(rhs.m_D3D_Resource)),
		m_ClearColor(rhs.m_ClearColor),
		m_ClearDepth(rhs.m_ClearDepth),
		m_ClearStencil(rhs.m_ClearStencil)
	{ }


	Resource& Resource::operator=(const Resource& rhs)
	{
		Reset();
		SetName(rhs.GetName());
		m_ClearColor = rhs.m_ClearColor;
		m_ClearDepth = rhs.m_ClearDepth;
		m_ClearStencil = rhs.m_ClearStencil;

		// register resource data map
		m_D3D_Resource = rhs.m_D3D_Resource;
		ResourceDataMap::RegisterResource(m_D3D_Resource.Get(), GetName(), D3D12_RESOURCE_STATE_COMMON);

		return *this;
	}
	Resource& Resource::operator=(Resource&& rhs)
	{
		// deregister resource data map
		Reset();
		SetName(rhs.GetName());
		m_ClearColor = rhs.m_ClearColor;
		m_ClearDepth = rhs.m_ClearDepth;
		m_ClearStencil = rhs.m_ClearStencil;
		m_D3D_Resource = std::move(rhs.m_D3D_Resource);

		return *this;
	}
	Resource::~Resource()
	{
		ResourceDataMap::DeRegisterResource(m_D3D_Resource.Get());
	}
	void Resource::CreateResource(
		const D3D12_RESOURCE_DESC& desc,
		D3D12_CLEAR_VALUE* clear_value,
		D3D12_RESOURCE_STATES init_state)
	{
		Reset();

		if (clear_value)
		{
			m_ClearColor.r = clear_value->Color[0];
			m_ClearColor.r = clear_value->Color[1];
			m_ClearColor.r = clear_value->Color[2];
			m_ClearColor.r = clear_value->Color[3];
			m_ClearDepth = clear_value->DepthStencil.Depth;
			m_ClearStencil = clear_value->DepthStencil.Stencil;
		}

		ComPtr<ID3D12Resource> d3d_resource;
		GetD3DDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			init_state,
			clear_value,
			IID_PPV_ARGS(d3d_resource.GetAddressOf()));
		RE_LOG_INFO("Create {0} : {1}", GetName(), (uint64_t)d3d_resource.Get());
		//
		SetD3DResource(d3d_resource, init_state);
	}
	void Resource::CreateResource(
		const D3D12_HEAP_PROPERTIES& heap_properties,
		D3D12_HEAP_FLAGS heap_flags,
		const D3D12_RESOURCE_DESC& desc,
		D3D12_RESOURCE_STATES init_state,
		D3D12_CLEAR_VALUE* clear_value)
	{
		Reset();

		if (clear_value)
		{
			m_ClearColor.r = clear_value->Color[0];
			m_ClearColor.r = clear_value->Color[1];
			m_ClearColor.r = clear_value->Color[2];
			m_ClearColor.r = clear_value->Color[3];
			m_ClearDepth = clear_value->DepthStencil.Depth;
			m_ClearStencil = clear_value->DepthStencil.Stencil;
		}

		ComPtr<ID3D12Resource> d3d_resource;
		GetD3DDevice()->CreateCommittedResource(
			&heap_properties,
			heap_flags,
			&desc,
			init_state,
			nullptr,
			IID_PPV_ARGS(d3d_resource.GetAddressOf()));

		RE_LOG_INFO("Create {0} : {1}", GetName(), (uint64_t)d3d_resource.Get());
		SetD3DResource(d3d_resource, init_state);
	}
	void Resource::SetD3DResource(Microsoft::WRL::ComPtr<ID3D12Resource> d3d_resource, D3D12_RESOURCE_STATES init_State)
	{
		Reset();
		m_D3D_Resource = d3d_resource;
		ResourceDataMap::RegisterResource(m_D3D_Resource.Get(), GetName(), init_State);
	}
	void Resource::Resize(uint64_t width, uint32_t height)
	{
		if (!IsVaild())
			return;
		width  = std::max<uint64_t>(width, 1);
		height = std::max<uint32_t>(height, 1);
		auto desc = GetDesc();
		desc.Width  = width;
		desc.Height = height;

		SetDesc(desc);
	}
	void Resource::Reset()
	{
		if (!IsVaild())
			return;

		ResourceDataMap::DeRegisterResource(m_D3D_Resource.Get());
		m_D3D_Resource.Reset();
	}
	bool Resource::IsVaild() const
	{
		return m_D3D_Resource != nullptr;
	}
	const _ResourceData* Resource::GetData() const
	{
		return ResourceDataMap::GetData(m_D3D_Resource.Get());
	}
	D3D12_RESOURCE_DESC Resource::GetDesc() const
	{
		if (m_D3D_Resource)
			return m_D3D_Resource->GetDesc();

		return D3D12_RESOURCE_DESC();
	}
	void Resource::SetDesc(const D3D12_RESOURCE_DESC& desc)
	{
		if (m_D3D_Resource == nullptr)
			return;
		D3D12_CLEAR_VALUE clear_value;
		clear_value.Format = desc.Format;
		clear_value.Color[0] = m_ClearColor.r;
		clear_value.Color[1] = m_ClearColor.g;
		clear_value.Color[2] = m_ClearColor.b;
		clear_value.Color[3] = m_ClearColor.a;
		clear_value.DepthStencil.Depth = m_ClearDepth;
		clear_value.DepthStencil.Stencil = m_ClearStencil;
		CreateResource(desc, &clear_value);
	}
	uint32_t Resource::GetRefCount() const
	{
		auto data = GetData();
		if (data)
			data->RefCount;

		return 0;
	}
	const Color& Resource::GetClearColor() const
	{
		return m_ClearColor;
	}
	float Resource::GetClearDepth() const
	{
		return m_ClearDepth;
	}
	uint8_t Resource::GetClearStencil() const
	{
		return m_ClearStencil;
	}
	ID3D12Resource* Resource::GetD3DResource() const
	{
		return m_D3D_Resource.Get();
	}
	void Resource::SetName(const std::string& name)
	{
		ReObject::SetName(name);
		if (m_D3D_Resource)
		{
			ResourceDataMap::SetResourceName(m_D3D_Resource.Get(), GetName());
		}
	}

	Texture::Texture() : Resource("Texture") { }
	Texture::Texture(const std::string& name) : Resource(name) { }


	D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetSrv() const
	{
		size_t seed = 0;
		if(m_SrvDesc) 
			std::hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*m_SrvDesc);

		std::lock_guard<std::mutex> lock(GetData()->SrvMutex);
		auto& SrvDescriptorHandles = GetData()->SrvDescriptorHandles;

		auto iter = SrvDescriptorHandles.find(seed);

		// 없다면 생성
		if (iter == SrvDescriptorHandles.end())
		{
			auto handle = GetRenderDevice()->SrvAllocate();
			GetD3DDevice()->CreateShaderResourceView(GetD3DResource(),
				m_SrvDesc.get(), handle.CPU());

			SrvDescriptorHandles.emplace(seed, std::move(handle));
		}

		return SrvDescriptorHandles.at(seed).CPU();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetRtv() const
	{
		size_t seed = 0;
		if(m_RtvDesc)
			std::hash<D3D12_RENDER_TARGET_VIEW_DESC>{}(*m_RtvDesc);

		std::lock_guard<std::mutex> lock(GetData()->RtvMutex);
		auto& RtvDescriptorHandles = GetData()->RtvDescriptorHandles;

		auto iter = RtvDescriptorHandles.find(seed);

		if (iter == RtvDescriptorHandles.end())
		{
			auto handle = GetRenderDevice()->RtvAllocate();
			GetD3DDevice()->CreateRenderTargetView(GetD3DResource(),
				m_RtvDesc.get(), handle.CPU());

			RtvDescriptorHandles.emplace(seed, std::move(handle));
		}

		return RtvDescriptorHandles.at(seed).CPU();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetDsv() const
	{
		size_t seed = 0; 
		if(m_DsvDesc) 
			std::hash<D3D12_DEPTH_STENCIL_VIEW_DESC>{}(*m_DsvDesc);
		std::lock_guard<std::mutex> lock(GetData()->DsvMutex);
		auto& DsvDescriptorHandles = GetData()->DsvDescriptorHandles;
		auto iter = DsvDescriptorHandles.find(seed);

		if (iter == DsvDescriptorHandles.end())
		{
			auto handle = GetRenderDevice()->DsvAllocate();

			GetD3DDevice()->CreateDepthStencilView(GetD3DResource(),
				m_DsvDesc.get(), handle.CPU());

			DsvDescriptorHandles.emplace(seed, std::move(handle));
		}
		return DsvDescriptorHandles.at(seed).CPU();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUav() const
	{
		size_t seed = 0;
		if(m_UavDesc)
			std::hash<D3D12_UNORDERED_ACCESS_VIEW_DESC>{}(*m_UavDesc);

		std::lock_guard<std::mutex> lock(GetData()->UavMutex);
		auto& UavDescriptorHandles = GetData()->UavDescriptorHandles;

		auto iter = UavDescriptorHandles.find(seed);

		if (iter == UavDescriptorHandles.end())
		{
			auto handle = GetRenderDevice()->UavAllocate();

			GetD3DDevice()->CreateUnorderedAccessView(
				GetD3DResource(), nullptr, m_UavDesc.get(), handle.CPU());

			UavDescriptorHandles.emplace(seed, std::move(handle));
		}
		return UavDescriptorHandles.at(seed).CPU();
	}
	void Texture::SetSrvDesc(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc)
	{
		if (m_SrvDesc)
			m_SrvDesc.reset();

		m_SrvDesc = std::make_shared<D3D12_SHADER_RESOURCE_VIEW_DESC>(desc);
	}
	void Texture::SetSrvDescNull()
	{
		if (m_SrvDesc)
			m_SrvDesc.reset();
		m_SrvDesc = nullptr;
	}
	void Texture::SetRtvDesc(const D3D12_RENDER_TARGET_VIEW_DESC& desc)
	{
		if (m_RtvDesc)
			m_RtvDesc.reset();

		m_RtvDesc = std::make_shared<D3D12_RENDER_TARGET_VIEW_DESC>(desc);
	}
	void Texture::SetRtvDescNull()
	{
		if (m_RtvDesc)
			m_RtvDesc.reset();
		m_RtvDesc = nullptr;
	}


	void Texture::SetDsvDesc(const D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
	{
		if (m_DsvDesc)
			m_DsvDesc.reset();

		m_DsvDesc = std::make_shared<D3D12_DEPTH_STENCIL_VIEW_DESC>(desc);
	}
	void Texture::SetDsvDescNull()
	{
		if (m_DsvDesc)
			m_DsvDesc.reset();
		m_DsvDesc = nullptr;
	}

	void Texture::SetUavDesc(const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
	{
		if (m_UavDesc)
			m_UavDesc.reset();

		m_UavDesc = std::make_shared<D3D12_UNORDERED_ACCESS_VIEW_DESC>(desc);
	}
	void Texture::SetUavDescNull()
	{
		if (m_UavDesc)
			m_UavDesc.reset();

		m_UavDesc = nullptr;
	}


	Buffer::Buffer() : Resource("Buffer")
	{
		
	}
	Buffer::Buffer(const std::string& name) : Resource(name)
	{

	}


	VertexBuffer::VertexBuffer() :
		Buffer("VertexBuffer") ,
		m_NumVertices(0),
		m_VertexStride(0)
	{

	}
	VertexBuffer::VertexBuffer(const std::string& name) : 
		Buffer(name),
		m_NumVertices(0),
		m_VertexStride(0)
	{

	}

	void VertexBuffer::CreateView(uint32_t numVertices, uint32_t vertexStride)
	{
		m_NumVertices = numVertices;
		m_VertexStride = vertexStride;

		m_VertexBufferView.BufferLocation = GetD3DResource()->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes    = m_NumVertices * m_VertexStride;
		m_VertexBufferView.StrideInBytes  = m_VertexStride;
	}

	IndexBuffer::IndexBuffer() :
		Buffer("IndexBuffer"),
		m_NumIndicies(0),
		m_IndexFormat(DXGI_FORMAT_UNKNOWN)
	{

	}
	IndexBuffer::IndexBuffer(const std::string& name) :
		Buffer(name),
		m_NumIndicies(0),
		m_IndexFormat(DXGI_FORMAT_UNKNOWN)
	{

	}
    void IndexBuffer::CreateView(uint32_t numIndicies, uint32_t indexSize)
	{
		assert(indexSize == 2 || indexSize == 4 &&
			"Indices must be 16, or 32-bit integers.");

		m_NumIndicies = numIndicies;
		m_IndexFormat = (indexSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;



		m_IndexBufferView.BufferLocation = GetD3DResource()->GetGPUVirtualAddress();
		m_IndexBufferView.Format = m_IndexFormat;
		m_IndexBufferView.SizeInBytes = indexSize * numIndicies;

	}
}