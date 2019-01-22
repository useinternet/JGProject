#pragma once
#include"DxCommon/DxCommon.h"
namespace JGRC
{
	typedef GeometryGenerator::Vertex Vertex;
	typedef std::vector<Vertex> MeshVertex;
	typedef std::vector<std::uint32_t> MeshIndex;
	class RCORE_EXPORT JGMesh
	{
	private:
		std::unique_ptr<MeshGeometry> m_Mesh;
		SubmeshGeometry m_DumpSubMesh;
		MeshVertex m_Vertex;
		MeshIndex  m_Index;
		UINT m_CurrStartIndexLocation = 0;
		UINT m_CurrBaseVertexLocation = 0;
	public:
		JGMesh(const std::string& name);

	public:
		void AddFbxMeshArg(const std::string& path);
		void AddMeshArg(const std::string& name, const MeshVertex& vertex, const MeshIndex& index);
		void AddBoxArg(const std::string& name,  float width,  float height,  float depth,  std::uint32_t numSubdivision);
		void AddSphereArg(const std::string& name, float radius, UINT32 numSubdivision);
		void AddSphereArg(const std::string& name, float radius, UINT32 sliceCount, UINT32 stackCount);
		void AddCylinderArg(const std::string& name, float bottomRadius,  float TopRadius, 
			float height, UINT32 sliceCount, UINT32 stackCount);
		void AddGridArg(const std::string& name, float width, float depth, std::uint32_t m, std::uint32_t n);
		void AddQuadArg(const std::string& name, float  x, float y, float w, float h, float depth);
		const std::string& GetName() const { return m_Mesh->Name; }

		virtual void CreateMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
		MeshGeometry* Data() { return m_Mesh.get(); }


		const SubmeshGeometry& GetMeshArg_c(const std::string& name) const;
		SubmeshGeometry& GetMeshArg(const std::string& name);
	};
}