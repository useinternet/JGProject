#pragma once
#include"DxCommon/DxCommon.h"


namespace JGRC
{
	class JGBaseMesh;
	class JGStaticMesh;
	class JGSkeletalMesh;
	class RCORE_EXPORT Interface_SceneMesh
	{
		friend class Interface_Scene;
		JGBaseMesh* m_Data = nullptr;
	public:
		Interface_SceneMesh() = default;
		Interface_SceneMesh(JGBaseMesh* mesh);
		virtual ~Interface_SceneMesh() {}
	};

	class RCORE_EXPORT Interface_SceneStaticMesh : public Interface_SceneMesh
	{
		JGStaticMesh* m_StaticData  = nullptr;
	public:
		Interface_SceneStaticMesh() = default;
		Interface_SceneStaticMesh(JGBaseMesh* mesh);
	public:
		Interface_SceneStaticMesh* operator->()
		{
			return this;
		}
	public:
		void AddStaticMeshArg(const std::string& path);

		void AddBoxArg(
			const std::string& name,
			float width,
			float height,
			float depth,
			std::uint32_t numSubdivision);

		void AddSphereArg(
			const std::string& name,
			float radius,
			UINT32 numSubdivision);

		void AddSphereArg(
			const std::string& name,
			float radius,
			UINT32 slicCount,
			UINT32 stackCount);

		void AddCylinderArg(
			const std::string& name,
			float bottomRadius,
			float topRadius,
			float height,
			UINT32 sliceCount,
			UINT32 stackCount);

		void AddGridArg(
			const std::string& name, 
			float width,
			float depth, 
			std::uint32_t m,
			std::uint32_t n);

		void AddQuadArg(
			const std::string& name,
			float  x, 
			float y, 
			float w, 
			float h, 
			float depth);
	};
	class RCORE_EXPORT Interface_SceneSkeletalMesh : public Interface_SceneMesh
	{
		JGSkeletalMesh* m_SkeletalData = nullptr;
	public:
		Interface_SceneSkeletalMesh() = default;
		Interface_SceneSkeletalMesh(JGBaseMesh* mesh);
	public:
		Interface_SceneSkeletalMesh* operator->()
		{
			return this;
		}
	public:
		void AddSkeletalMeshArg(const std::string& name);
	};


	typedef Interface_SceneMesh IF_Mesh;
	typedef Interface_SceneStaticMesh IF_StaticMesh;
	typedef Interface_SceneSkeletalMesh IF_SkeletalMesh;
}