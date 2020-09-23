#include "pch.h"
#include "DX12_SceneObject.h"
#include "DX12_Material.h"
#include "DX12_GraphicsRenderer.h"
#include "../GraphicsIF.h"

using namespace std;

namespace DX12
{



	void DX12_SceneObject::SetMobility(GE::ObjectMobility mobility)
	{
		m_Mobility = mobility;
	}

	GE::ObjectMobility DX12_SceneObject::GetMobility() const
	{
		return m_Mobility;
	}

	void DX12_SceneObject::SetMeshType(GE::MeshType type)
	{
		
		bool is_diff = m_MeshType != type;
		m_MeshType = type;
		if (is_diff && m_vDataPtr.IsValid() && m_iDataPtr.IsValid())
		{
			BindMesh(
				m_vDataPtr.ptr, m_vDataPtr.size, m_vDataPtr.count,
				(uint32_t*)m_iDataPtr.ptr,  m_iDataPtr.count);
		}
	}

	GE::MeshType DX12_SceneObject::GetMeshType() const
	{
		return m_MeshType;
	}


	GE::ObjectType DX12_SceneObject::GetObjectType() const
	{
		return m_ObjectType;
	}

	void DX12_SceneObject::SetName(const std::wstring& name)
	{
		m_Name = name;
	}

	const std::wstring& DX12_SceneObject::GetName() const
	{
		return m_Name;
	}

	GE::MaterialInstance* DX12_SceneObject::GetMaterial() const
	{
		return m_Material;
	}

	void DX12_SceneObject::BindMesh(StaticVertex* vData, size_t vElementCount, uint32_t* iData, size_t iElementCount)
	{
		m_ObjectType = GE::ObjectType::SDStatic;
		BindMesh(vData, sizeof(StaticVertex), vElementCount, iData, iElementCount);
	}

	void DX12_SceneObject::BindMesh(SkinnedVertex* vData, size_t vElementCount, uint32_t* iData, size_t iElementCount)
	{
		m_ObjectType = GE::ObjectType::SDSkinned;
		BindMesh(vData, sizeof(SkinnedVertex), vElementCount, iData, iElementCount);
	}

	void DX12_SceneObject::BindMesh(PaperVertex* vData, size_t vElementCount, uint32_t* iData, size_t iElementCount)
	{
		m_ObjectType = GE::ObjectType::SDPaper;
		BindMesh(vData, sizeof(PaperVertex), vElementCount, iData, iElementCount);
	}

	void DX12_SceneObject::BindMesh(
		void* vData, size_t vElementSize, size_t vElementCount, 
		uint32_t* iData, size_t iElementCount)
	{

		if (m_vDataPtr.IsValid() || m_iDataPtr.IsValid()) UnBindMesh();

		m_vDataPtr.ptr = vData;
		m_vDataPtr.size = vElementSize;
		m_vDataPtr.count = vElementCount;

		m_iDataPtr.ptr = iData;
		m_iDataPtr.size = sizeof(uint32_t);
		m_iDataPtr.count = iElementCount;

		switch (m_MeshType)
		{
		case GE::MeshType::Static:
		{
			m_vBuffer.Create(m_Name + TT("_SceneObject_VertexBuffer"), m_vDataPtr.BtSize());
			m_iBuffer.Create(m_Name + TT("_SceneObject_IndexBuffer"), m_iDataPtr.BtSize());

			auto cmdKey = DX12::DXDevice::RequestGraphicsCommandKey();
			DXCommand::CopyVertexBuffer(cmdKey, m_vBuffer, m_vDataPtr.ptr, m_vDataPtr.size, m_vDataPtr.count);
			DXCommand::CopyIndexBuffer(cmdKey, m_iBuffer, m_iDataPtr.ptr, m_iDataPtr.count);
		}
			break;
		case GE::MeshType::Dynamic:
			if (m_vBuffer.IsValid()) m_vBuffer = VertexBuffer();
			if (m_iBuffer.IsValid()) m_iBuffer = IndexBuffer();
			break;
		}
	}
	void DX12_SceneObject::UnBindMesh()
	{
		if (m_vBuffer.IsValid()) m_vBuffer = VertexBuffer();
		if (m_iBuffer.IsValid()) m_iBuffer = IndexBuffer();

		m_vDataPtr.ptr = nullptr;
		m_vDataPtr.size = 0;
		m_vDataPtr.count = 0;


		m_iDataPtr.ptr = nullptr;
		m_iDataPtr.size = 0;
		m_iDataPtr.count = 0;

	}
	void DX12_SceneObject::BindMaterial(GE::Material* m)
	{
		if (m_Material && m_Material->GetOwnerMaterial() == m) return;
		if (m_Material) UnBindMaterial();

		if (m == nullptr) {
			m = GraphicsIF::GetMaterial(EMPTY_MATERIAL_3D).Get();
		}
		m_Material = m->CreateMaterialInstance();
		
		//TODO
	}
	void DX12_SceneObject::UnBindMaterial()
	{
		if (m_Material == nullptr) return;
		m_Material->GetOwnerMaterial()->DeleteMaterialInstance(m_Material);
		m_Material = nullptr;
		//TODO
	}
	void DX12_SceneObject::BindInstanceData(
		SDObject* data, size_t instanceCount)
	{
		m_Instances.ptr   = data;
		m_Instances.size  = sizeof(SDObject);
		m_Instances.count = instanceCount;
	}
	void DX12_SceneObject::UnBindInstanceData()
	{
		m_Instances.ptr   = nullptr;
		m_Instances.size  = 0;
		m_Instances.count = 0;

	}
	void DX12_SceneObject::BindAnimationData(CBAnimData* data)
	{
		m_AnimCBData.ptr = data;
		m_AnimCBData.size = sizeof(CBAnimData);
		m_AnimCBData.count = 1;
	}
	void DX12_SceneObject::UnBindAnimationData()
	{
		m_AnimCBData.ptr = nullptr;
		m_AnimCBData.size = 0;
		m_AnimCBData.count = 0;
	}
	void DX12_SceneObject::DrawCall(void* userData, bool isShadow)
	{
		GraphicsCommandKeyPtr cmdKey = *((GraphicsCommandKeyPtr*)userData);
		if (m_Instances.size == 0) {
			GELOG_ERROR("인스턴스 데이터가 존재하지 않습니다.");
			return;
		}


		if (m_ObjectType == GE::ObjectType::SDSkinned && m_MeshType == GE::MeshType::Static)
		{
			GELOG_ERROR("오브젝트 타입이 Skinned 인데  MeshType이 Static입니다.");
			return;
		}
		else if (m_Material && !isShadow)
		{
			auto matProperty = m_Material->GetOwnerMaterial()->GetMaterialProperty();
			if ((m_ObjectType == GE::ObjectType::SDSkinned || m_ObjectType == GE::ObjectType::SDStatic) &&
				(matProperty.rendererMode == GE::RendererMode::Paper || matProperty.rendererMode == GE::RendererMode::GUI))
			{
				GELOG_ERROR("오브젝트 타입이 Skinned 이거나 Static 인데 렌더러 모드가 Paper이거나 GUI입니다.");
				return;
			}
			else if (m_ObjectType == GE::ObjectType::SDPaper && matProperty.rendererMode == GE::RendererMode::_3D)
			{
				GELOG_ERROR("오브젝트 타입이 Paper인데 렌더러 모드가 3D입니다.");
				return;
			}

			std::vector<Texture> tArray;
			m_Material->GetTextureData(&tArray);
			std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC*> desc(tArray.size(), nullptr);
		
			DXCommand::BindTextures(cmdKey,
				RootParam::BindTexture,
				tArray.data(), (void**)desc.data(), tArray.size());
		}
		 

		if (!isShadow)
		{
			//Material Bind
			DXCommand::BindDynamicConstantBuffer(
				cmdKey, RootParam::Material,
				m_Material->BtData(), m_Material->BtSize());
		}
		



		// Object Bind
		DXCommand::BindDynamicStructuredBuffer(
			cmdKey, RootParam::Object,
			m_Instances.ptr, m_Instances.size, m_Instances.count);

		size_t vertexSize = m_vDataPtr.size;
		// AnimData Bind
		switch (m_ObjectType)
		{
		case GE::ObjectType::SDSkinned:
			if (!m_AnimCBData.IsValid())
			{
				CBAnimData animData;
				animData.Reset();
				DXCommand::BindDynamicConstantBuffer(cmdKey,
					RootParam::AnimData,
					&animData, sizeof(CBAnimData));
			}
			else
			{
				DXCommand::BindDynamicConstantBuffer(cmdKey,
					RootParam::AnimData,
					m_AnimCBData.ptr, m_AnimCBData.size);
			}
			break;
		}

		//
		switch (m_MeshType)
		{
		case GE::MeshType::Static:
		{
			if (!m_vBuffer.IsValid() || !m_iBuffer.IsValid())
			{
				GELOG_ERROR("VertexBuffer 나 IndexBuffer가  유효하지 않습니다.");
				return;
			}
			DXCommand::SetPrimitiveTopology(cmdKey, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			DXCommand::BindVertexBuffer(cmdKey, m_vBuffer);
			DXCommand::BindIndexBuffer(cmdKey, m_iBuffer);
			DXCommand::DrawIndexed(cmdKey, m_iDataPtr.count, m_Instances.count);
		}
			break;
		case GE::MeshType::Dynamic:
		{
			if (!m_vDataPtr.IsValid() || !m_iDataPtr.IsValid())
			{
				GELOG_ERROR("VertexData 나 IndexData가  유효하지 않습니다.");
				return;
			}
			DXCommand::SetPrimitiveTopology(cmdKey, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			DXCommand::BindDynamicVertexBuffer(cmdKey, 
				m_vDataPtr.ptr, m_vDataPtr.size, m_vDataPtr.count);
			
			DXCommand::BindDynamicIndexBuffer(cmdKey, (uint32_t*)m_iDataPtr.ptr, m_iDataPtr.count);
			DXCommand::DrawIndexed(cmdKey, m_iDataPtr.count, m_Instances.count);

		}
			break;
		}



	}
}

