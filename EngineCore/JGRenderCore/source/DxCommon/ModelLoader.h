#pragma once
#include"DxCommon.h"
#include"GeometryGenerator.h"


struct aiNode;
struct aiMesh;
struct aiScene;
namespace JGRC
{
	class RCORE_EXPORT ModelLoader
	{
		UINT m_FaceOffset = 0;
	public:
		bool Success   = true;
		bool IsTangent = true;
	public:
		ModelLoader(const std::string& path, GeometryGenerator::MeshData* MeshData);
	private:
		void ProcessNode(aiNode* node, const aiScene* scene, GeometryGenerator::MeshData* MeshData);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene, GeometryGenerator::MeshData* meshData);

	};
}