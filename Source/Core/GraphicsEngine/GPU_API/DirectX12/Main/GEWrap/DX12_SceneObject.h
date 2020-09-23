#pragma once
#include "SceneObject.h"
#include "Material.h"
#include "DirectXApi.h"


namespace DX12
{
	REGISTER_GRAPHICS_INTERFACE_SCENEOBJECT(DX12_SceneObject)


	class DX12_SceneObject : public GE::SceneObject
	{
	public:
		virtual void SetMobility(GE::ObjectMobility mobility) override;
		virtual GE::ObjectMobility GetMobility() const override;

		virtual void SetMeshType(GE::MeshType type) override;
		virtual GE::MeshType GetMeshType() const override;

		virtual GE::ObjectType GetObjectType() const override;


		virtual void SetName(const std::wstring& name) override;
		virtual const std::wstring& GetName() const override;

		virtual GE::MaterialInstance* GetMaterial() const override;
		virtual void BindMesh(
			StaticVertex* vData, size_t vElementCount, uint32_t* iData, size_t iElementCount) override;
		virtual void BindMesh(
			SkinnedVertex* vData, size_t vElementCount, uint32_t* iData, size_t iElementCount) override;
		virtual void BindMesh(
			PaperVertex* vData, size_t vElementCount, uint32_t* iData, size_t iElementCount) override;


		// moveMode
	    void BindMesh(
			void* vData, size_t vElementSize, size_t vElementCount, 
			uint32_t* iData, size_t iElementCount);

		virtual void UnBindMesh() override;


		virtual void BindMaterial(GE::Material* m) override;
		virtual void UnBindMaterial() override;

		virtual void BindInstanceData(SDObject* data, size_t instanceCount) override;
		virtual void UnBindInstanceData() override;

		virtual void BindAnimationData(CBAnimData* data) override;
		virtual void UnBindAnimationData() override;


		virtual void DrawCall(void* userData = nullptr, bool isShadow = false) override;

		virtual ~DX12_SceneObject() override {
			m_vBuffer = VertexBuffer();
			m_iBuffer = IndexBuffer();
		}
	private:
		struct DataPtr
		{
			void*  ptr = nullptr;
			size_t size = 0;
			size_t count = 0;

			bool IsValid() const {
				return ptr != nullptr;
			}
			size_t BtSize() const {
				return size * count;
			}
		};
		
	private:
		GE::ObjectMobility m_Mobility   = GE::ObjectMobility::Static;
		GE::MeshType       m_MeshType   = GE::MeshType::Static;
		GE::ObjectType     m_ObjectType = GE::ObjectType::SDStatic;

		GE::MaterialInstance* m_Material = nullptr;

		DataPtr m_vDataPtr;
		DataPtr m_iDataPtr;

		VertexBuffer m_vBuffer;
		IndexBuffer  m_iBuffer;


		std::wstring m_Name;
		DataPtr m_Instances;
		DataPtr m_AnimCBData;

	};
}