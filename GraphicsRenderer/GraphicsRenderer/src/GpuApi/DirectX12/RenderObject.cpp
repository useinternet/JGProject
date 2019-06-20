#include"pch.h"
#include"RenderObject.h"
#include"RootSignatureCache.h"
#include"Mesh.h"
#include"Material.h"
#include"Commander.h"
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
		void RenderObject::BindMaterial(Material* mat)
		{
			if (m_Material)
				m_Material.reset();
			m_Material = std::make_unique<Material>();

			if (mat)
			{
				*m_Material = *mat;
			}
		}
		void RenderObject::BindConstantBufferAtGPU(GraphicsCommander* commander)
		{
			if (!m_ObjectCBs.empty())
			{
				commander->SetDynamicStructuredBuffer(G_Common_RootParam_ObjectCB, (uint32_t)m_ObjectCBs.size(),
					sizeof(ObjectCB), m_ObjectCBs.data());
			}

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
		Mesh* RenderObject::GetMesh()
		{
			return m_Mesh.get();
		}
		Material* RenderObject::GetMaterial()
		{
			if (!m_Material)
				m_Material = std::make_unique<Material>();
			return m_Material.get();
		}
		const Mesh* RenderObject::GetMesh() const
		{
			return m_Mesh.get();
		}
		const Material* RenderObject::GetMaterial() const
		{
			if (!m_Material)
				return nullptr;
			return m_Material.get();
		}
	}
}