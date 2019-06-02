#include"pch.h"
#include"RenderObject.h"
#include"RootSignatureCache.h"
#include"Mesh.h"
namespace GR
{
	namespace Dx12
	{
		RenderObject::RenderObject()
		{

		}
		void RenderObject::BindMesh(Mesh* mesh)
		{
			if (m_Mesh)
				m_Mesh.reset();
			m_Mesh = std::make_unique<Mesh>();
			if (mesh)
			{
				*m_Mesh = *mesh;
			}
		}
		void RenderObject::BindTexture(RenderObject::ETextureSlot slot, const Texture& texture)
		{
			m_Textures[slot] = texture;
		}
		void RenderObject::BindCubeTexture(ETextureSlot slot, const Texture& texture)
		{
			m_CubeTextures[slot] = texture;


			auto desc = m_CubeTextures[slot]->GetDesc();
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.Format = desc.Format;
	
			srvDesc.TextureCube.MipLevels = desc.MipLevels;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

			m_CubeTextures[slot].SetSRVDesc(&srvDesc);

		}
		void RenderObject::AddInstance(const ObjectCB& objectCB)
		{
			m_ObjectCBs.push_back(objectCB);
		}
		void RenderObject::SetInstance(uint32_t ID, const ObjectCB& objectCB)
		{
			m_ObjectCBs[ID] = objectCB;
		}
		ObjectCB& RenderObject::GetInstance(uint32_t ID)
		{
			return m_ObjectCBs[ID];
		}
		const ObjectCB& RenderObject::GetInstance(uint32_t ID) const
		{
			return m_ObjectCBs[ID];
		}

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RenderObject::GetTextureSRV() const
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles(G_Common_Value_NumTextureSlot);
			for (uint32_t i = 0; i < RenderObject::NUM_TEXTURESLOT; ++i)
			{
				if (m_Textures[i].IsValid())
				{
					handles[i] = m_Textures[i].GetSRV();
				}
				else
				{
					handles[i].ptr = 0;
				}

			}
			return handles;
		}
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RenderObject::GetCubeTextureSRV() const
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles(G_Common_Value_NumTextureSlot);
			for (uint32_t i = 0; i < RenderObject::NUM_TEXTURESLOT; ++i)
			{
				if (m_CubeTextures[i].IsValid())
				{
					handles[i] = m_CubeTextures[i].GetSRV();
				}
				else
				{
					handles[i].ptr = 0;
				}

			}
			return handles;

		}
	}
}