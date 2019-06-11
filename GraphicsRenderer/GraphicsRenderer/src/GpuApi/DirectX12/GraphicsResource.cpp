#include"pch.h"
#include"GraphicsResource.h"
#include"GraphicsDevice.h"

#include"Dx12Helper.h"

using namespace std;


namespace GR
{
	namespace Dx12
	{
		// ----     GPUResource Class     ----//



		// ----     VertexBuffer Class     ----//

		void VertexBuffer::CreateView(uint32_t numElements, uint32_t elementSize)
		{
			m_NumVertices = numElements;
			m_VertexStride = elementSize;

			m_VertexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
			m_VertexBufferView.SizeInBytes    = m_NumVertices * m_VertexStride;
			m_VertexBufferView.StrideInBytes  = m_VertexStride;
		}
		// ----     IndexBuffer Class     ----//
		void IndexBuffer::CreateView(uint32_t numElements, uint32_t elementSize)
		{
			assert(elementSize == 2 || elementSize == 4 && "Index's format must be 16 or 32 bit integer");


			m_NumIndices = numElements;
			m_IndexFormat = (elementSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;


			m_IndexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
			m_IndexBufferView.SizeInBytes = numElements * elementSize;
			m_IndexBufferView.Format = m_IndexFormat;
		}
		// ----     Texture Class     ----//
		D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetSRV(D3D12_SHADER_RESOURCE_VIEW_DESC* desc) const
		{
			uint32_t hashCode = 0;
			if (desc)
			{
				hashCode = (uint32_t)Hash::HashState<D3D12_SHADER_RESOURCE_VIEW_DESC>(desc);
			}


			auto srv = m_SRVs.find(hashCode);
			if (srv == m_SRVs.end())
			{
				auto gh_device = GraphicsDevice::GetPointer();
				auto d3d_device = gh_device->GetD3DDevice();


				auto ptr = gh_device->Allocate(GraphicsDevice::SRV_UAV_CBV);
				d3d_device->CreateShaderResourceView(m_Resource.Get(),
					desc, ptr.GetDescriptorHandle());

				auto result = ptr.GetDescriptorHandle();
				m_SRVs[hashCode] = move(ptr);

				return result;
			}

			return srv->second.GetDescriptorHandle();
		}
		D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUAV(D3D12_UNORDERED_ACCESS_VIEW_DESC* desc) const
		{
			uint32_t hashCode = 0;
			if (desc)
			{
				hashCode = (uint32_t)Hash::HashState<D3D12_UNORDERED_ACCESS_VIEW_DESC>(desc);
			}

			auto uav = m_UAVs.find(hashCode);
			if (uav == m_UAVs.end())
			{
				auto gh_device = GraphicsDevice::GetPointer();
				auto d3d_device = gh_device->GetD3DDevice();


				auto ptr = gh_device->Allocate(GraphicsDevice::SRV_UAV_CBV);
				d3d_device->CreateUnorderedAccessView(m_Resource.Get(),nullptr,
					desc, ptr.GetDescriptorHandle());

				auto result = ptr.GetDescriptorHandle();
				m_UAVs[hashCode] = move(ptr);

				return result;
			}

			return uav->second.GetDescriptorHandle();

		}

		Texture::Texture(const Texture& copy) : GPUResource(copy)
		{
			if (m_CurrentSrvDesc)
				m_CurrentSrvDesc.reset();

			(copy.m_CurrentSrvDesc) ?
				m_CurrentSrvDesc = make_unique<D3D12_SHADER_RESOURCE_VIEW_DESC>(*copy.m_CurrentSrvDesc) :
				m_CurrentSrvDesc = nullptr;

			if (m_CurrentUavDesc)
				m_CurrentUavDesc.reset();

			(copy.m_CurrentUavDesc) ?
				m_CurrentUavDesc = make_unique<D3D12_UNORDERED_ACCESS_VIEW_DESC>(*copy.m_CurrentUavDesc) :
				m_CurrentUavDesc = nullptr;

			m_SRVs.clear();
			m_UAVs.clear();
		}
		Texture::Texture(Texture&& rhs) : GPUResource(rhs)
		{
			m_SRVs = move(m_SRVs);
			m_UAVs = move(m_UAVs);
			m_CurrentSrvDesc = move(rhs.m_CurrentSrvDesc);
			m_CurrentUavDesc = move(rhs.m_CurrentUavDesc);
		}
		Texture& Texture::operator=(const Texture& rhs)
		{
			GPUResource::operator=(rhs);

			if (m_CurrentSrvDesc)
				m_CurrentSrvDesc.reset();

			(rhs.m_CurrentSrvDesc) ?
				m_CurrentSrvDesc = make_unique<D3D12_SHADER_RESOURCE_VIEW_DESC>(*rhs.m_CurrentSrvDesc) :
				m_CurrentSrvDesc = nullptr;

			if (m_CurrentUavDesc)
				m_CurrentUavDesc.reset();

			(rhs.m_CurrentUavDesc) ?
				m_CurrentUavDesc = make_unique<D3D12_UNORDERED_ACCESS_VIEW_DESC>(*rhs.m_CurrentUavDesc) :
				m_CurrentUavDesc = nullptr;


			m_SRVs.clear();
			m_UAVs.clear();
			return *this;
		}
		Texture& Texture::operator=(Texture&& rhs)
		{
			GPUResource::operator=(rhs);
			m_SRVs = move(m_SRVs);
			m_UAVs = move(m_UAVs);
			m_CurrentSrvDesc = move(rhs.m_CurrentSrvDesc);
			m_CurrentUavDesc = move(rhs.m_CurrentUavDesc);

			return *this;
		}


		void Texture::SetSRVDesc(D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
		{
			if (desc)
			{
				if (!m_CurrentSrvDesc)
				{
					m_CurrentSrvDesc = make_unique<D3D12_SHADER_RESOURCE_VIEW_DESC>();
				}
				*m_CurrentSrvDesc = *desc;
			}
			else
			{
				if (m_CurrentSrvDesc)
				{
					m_CurrentSrvDesc.reset();
					m_CurrentSrvDesc = nullptr;
				}
			}
		}
		void Texture::SetUAVDesc(D3D12_UNORDERED_ACCESS_VIEW_DESC* desc)
		{
			if (desc)
			{
				if (!m_CurrentUavDesc)
				{
					m_CurrentUavDesc = make_unique<D3D12_UNORDERED_ACCESS_VIEW_DESC>();
				}
				*m_CurrentUavDesc = *desc;
			}
			else
			{
				if (m_CurrentUavDesc)
				{
					m_CurrentUavDesc.reset();
					m_CurrentUavDesc = nullptr;
				}
			}
		}
		D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetSRV() const {
			return GetSRV(m_CurrentSrvDesc.get());
		}
		D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUAV() const {
			return GetUAV(m_CurrentUavDesc.get());
		}




		// ----     Color Texture Class     ----//


		D3D12_CPU_DESCRIPTOR_HANDLE ColorTexture::GetRTV(D3D12_RENDER_TARGET_VIEW_DESC* desc) const
		{
			uint32_t hashCode = 0;
			if (desc)
			{
				hashCode = (uint32_t)Hash::HashState<D3D12_RENDER_TARGET_VIEW_DESC>(desc);
			}

			auto rtv = m_RTVs.find(hashCode);
			if (rtv == m_RTVs.end())
			{
				auto gh_device = GraphicsDevice::GetPointer();
				auto d3d_device = gh_device->GetD3DDevice();


				auto ptr = gh_device->Allocate(GraphicsDevice::RTV);
				d3d_device->CreateRenderTargetView(m_Resource.Get(),
					desc, ptr.GetDescriptorHandle());

				auto result = ptr.GetDescriptorHandle();
				m_RTVs[hashCode] = move(ptr);

				return result;
			}

			return rtv->second.GetDescriptorHandle();
		}


		ColorTexture::ColorTexture(const ColorTexture& copy) :
			Texture(copy), m_ClearColor(copy.m_ClearColor)
		{
			if (m_CurrentRtvDesc)
				m_CurrentRtvDesc.reset();

			(copy.m_CurrentRtvDesc) ?
				m_CurrentRtvDesc = make_unique<D3D12_RENDER_TARGET_VIEW_DESC>(*copy.m_CurrentRtvDesc) :
				m_CurrentRtvDesc = nullptr;

			m_RTVs.clear();
		}
		ColorTexture::ColorTexture(ColorTexture&& rhs) : 
			Texture(rhs), m_ClearColor(rhs.m_ClearColor)
		{
			m_RTVs = move(rhs.m_RTVs);
			m_CurrentRtvDesc = move(rhs.m_CurrentRtvDesc);
		}
		ColorTexture& ColorTexture::operator=(const ColorTexture& rhs)
		{
			Texture::operator=(rhs);
			if (m_CurrentRtvDesc)
				m_CurrentRtvDesc.reset();

			(rhs.m_CurrentRtvDesc) ?
				m_CurrentRtvDesc = make_unique<D3D12_RENDER_TARGET_VIEW_DESC>(*rhs.m_CurrentRtvDesc) :
				m_CurrentRtvDesc = nullptr;

			m_ClearColor = rhs.m_ClearColor;


			m_RTVs.clear();
			return *this;
		}
		ColorTexture& ColorTexture::operator=(ColorTexture&& rhs)
		{
			Texture::operator=(rhs);
			m_RTVs = move(rhs.m_RTVs);
			m_CurrentRtvDesc = move(rhs.m_CurrentRtvDesc);
			m_ClearColor = rhs.m_ClearColor;
			rhs.m_ClearColor = { 0.0f,0.0f,0.0f, 0.0f };
			return *this;
		}
		void ColorTexture::SetRTVDesc(D3D12_RENDER_TARGET_VIEW_DESC* desc)
		{
			if (desc)
			{
				if (!m_CurrentRtvDesc)
				{
					m_CurrentRtvDesc = make_unique<D3D12_RENDER_TARGET_VIEW_DESC>();
				}
				*m_CurrentRtvDesc = *desc;
			}
			else
			{
				if (m_CurrentRtvDesc)
				{
					m_CurrentRtvDesc.reset();
					m_CurrentRtvDesc = nullptr;
				}
			}
		}
		D3D12_CPU_DESCRIPTOR_HANDLE ColorTexture::GetRTV() const
		{
			return GetRTV(m_CurrentRtvDesc.get());

		}




		// ----     Depth Texture Class     ----//


		D3D12_CPU_DESCRIPTOR_HANDLE DepthTexture::GetDSV(D3D12_DEPTH_STENCIL_VIEW_DESC* desc) const
		{
			uint32_t hashCode = 0;
			if (desc)
			{
				hashCode = (uint32_t)Hash::HashState<D3D12_DEPTH_STENCIL_VIEW_DESC>(desc);
			}

			auto dsv = m_DSVs.find(hashCode);
			if (dsv == m_DSVs.end())
			{
				auto gh_device = GraphicsDevice::GetPointer();
				auto d3d_device = gh_device->GetD3DDevice();


				auto ptr = gh_device->Allocate(GraphicsDevice::DSV);
				d3d_device->CreateDepthStencilView(m_Resource.Get(),
					desc, ptr.GetDescriptorHandle());

				auto result = ptr.GetDescriptorHandle();
				m_DSVs[hashCode] = move(ptr);

				return result;
			}

			return dsv->second.GetDescriptorHandle();
		}
		DepthTexture::DepthTexture(const DepthTexture& copy) : 
			Texture(copy), 
			m_ClearDepth(copy.m_ClearDepth),
			m_ClearStencil(copy.m_ClearStencil)
		{
			if (m_CurrentDsvDesc)
				m_CurrentDsvDesc.reset();

			(copy.m_CurrentDsvDesc) ?
				m_CurrentDsvDesc = make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>(*copy.m_CurrentDsvDesc) :
				m_CurrentDsvDesc = nullptr;
			m_DSVs.clear();

		}
		DepthTexture::DepthTexture(DepthTexture&& rhs) : 
			Texture(rhs),
			m_ClearDepth(rhs.m_ClearDepth),
			m_ClearStencil(rhs.m_ClearStencil)
		{

			m_DSVs = move(rhs.m_DSVs);
			m_CurrentDsvDesc = move(rhs.m_CurrentDsvDesc);
			rhs.m_ClearDepth = 1.0f;
			rhs.m_ClearStencil = 0;
		}
		DepthTexture& DepthTexture::operator=(const DepthTexture& rhs)
		{
			Texture::operator=(rhs);
			if (m_CurrentDsvDesc)
				m_CurrentDsvDesc.reset();

			(rhs.m_CurrentDsvDesc) ?
				m_CurrentDsvDesc = make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>(*rhs.m_CurrentDsvDesc) :
				m_CurrentDsvDesc = nullptr;
			m_ClearDepth = rhs.m_ClearDepth;
			m_ClearStencil = rhs.m_ClearStencil;

			return *this;
		}
		DepthTexture& DepthTexture::operator=(DepthTexture&& rhs)
		{
			Texture::operator=(rhs);
			m_DSVs = move(rhs.m_DSVs);
			m_CurrentDsvDesc = move(rhs.m_CurrentDsvDesc);
			m_ClearDepth = rhs.m_ClearDepth;
			m_ClearStencil = rhs.m_ClearStencil;
			rhs.m_ClearDepth = 1.0f;
			rhs.m_ClearStencil = 0;
			m_DSVs.clear();
			return *this;
		}
		void DepthTexture::SetDSVDesc(D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
		{
			if (desc)
			{
				if (!m_CurrentDsvDesc)
				{
					m_CurrentDsvDesc = make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>();
				}
				*m_CurrentDsvDesc = *desc;
			}
			else
			{
				if (m_CurrentDsvDesc)
				{
					m_CurrentDsvDesc.reset();
					m_CurrentDsvDesc = nullptr;
				}
			}
		}
		D3D12_CPU_DESCRIPTOR_HANDLE DepthTexture::GetDSV() const
		{
			return GetDSV(m_CurrentDsvDesc.get());

		}
	}
}


