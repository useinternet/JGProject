#pragma once
#include"JGBaseMesh.h"


namespace JGRC
{
	typedef GeometryGenerator::Vertex        Vertex;
	typedef std::vector<Vertex>              MeshVertex;
	typedef std::vector<std::uint32_t>       MeshIndex;

	class RCORE_EXPORT JGStaticMesh : public JGBaseMesh
	{
	private:
		SubmeshGeometry m_DumpSubMesh;
		MeshVertex m_Vertex;
		MeshIndex  m_Index;

	public:
		virtual void CreateMesh(ID3D12GraphicsCommandList* CommandList) override;
	public:
		JGStaticMesh(const std::string& name);
		void AddFbxMeshArg(const std::string& path);
		void AddBoxArg    (const std::string& name, float width, float height, float depth, std::uint32_t numSubdivision);
		void AddSphereArg (const std::string& name, float radius, UINT32 numSubdivision);
		void AddSphereArg (const std::string& name, float radius, UINT32 sliceCount, UINT32 stackCount);
		void AddCylinderArg (const std::string& name, float bottomRadius, float TopRadius, float height, UINT32 sliceCount, UINT32 stackCount);
		void AddGridArg(const std::string& name, float width, float depth, std::uint32_t m, std::uint32_t n);
		void AddQuadArg(const std::string& name, float  x, float y, float w, float h, float depth);
	private:
		void AddMeshArg(const std::string& name, const MeshVertex& vertex, const MeshIndex& index);


	};
}



