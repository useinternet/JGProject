#pragma once




#include "GEObject.h"

/*
Class : SceneObject

ex : 렌더링할 품목 ( 머터리얼, 메시, 인스턴스 설정 )

// 스태틱, 다이나믹

*/

namespace GE
{
	enum class ObjectMobility
	{
		Static,
		Stationary,
		Movable
	};
	enum class MeshType
	{
		Static,
		Dynamic
	};
	enum class ObjectType
	{
		SDStatic,
		SDSkinned,
		SDPaper
	};
	class Material;
	class MaterialInstance;
	class GRAPHICSENGINE_API SceneObject : public GEObject
	{

	public:
		virtual void SetMobility(ObjectMobility mobility) = 0;
		virtual ObjectMobility GetMobility() const = 0;

		virtual void SetMeshType(MeshType type) = 0;
		virtual MeshType GetMeshType() const = 0;

		virtual ObjectType GetObjectType() const = 0;


		virtual void SetName(const std::wstring& name) = 0;
		virtual const std::wstring& GetName() const = 0;


		virtual MaterialInstance* GetMaterial() const = 0;


		virtual void BindMesh(
			StaticVertex* vData, size_t vElementCount, uint32_t* iData, size_t iElementCount) = 0;
		virtual void BindMesh(
			SkinnedVertex* vData, size_t vElementCount,  uint32_t* iData, size_t iElementCount) = 0;
		virtual void BindMesh(
			PaperVertex* vData, size_t vElementCount,  uint32_t* iData, size_t iElementCount) = 0;
		virtual void UnBindMesh() = 0;




		virtual void BindMaterial(Material* m) = 0;
		virtual void UnBindMaterial() = 0;


		virtual void BindInstanceData(SDObject* data, size_t instanceCount) = 0;
		virtual void UnBindInstanceData() = 0;

		virtual void BindAnimationData(CBAnimData* data) = 0;
		virtual void UnBindAnimationData() = 0;

		virtual void DrawCall(void* userData = nullptr) = 0;

		virtual ~SceneObject() {}
	};

	

}


