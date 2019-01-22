#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include"ModelLoader.h"

using namespace JGRC;
using namespace std;

ModelLoader::ModelLoader(const std::string& path, GeometryGenerator::MeshData* MeshData)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(path, 
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		Success = false;
	else
	{
		ProcessNode(pScene->mRootNode, pScene, MeshData);
	}

}
void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, GeometryGenerator::MeshData* MeshData)
{
	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		GeometryGenerator::MeshData data;
		ProcessMesh(mesh, scene, &data);

		MeshData->Vertices.insert(MeshData->Vertices.end(), data.Vertices.begin(), data.Vertices.end());
		MeshData->Indices32.insert(MeshData->Indices32.end(), data.Indices32.begin(), data.Indices32.end());
	}
	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		this->ProcessNode(node->mChildren[i], scene, MeshData);
	}

}
void ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, GeometryGenerator::MeshData* meshData)
{

	// ¡§¡°
	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		GeometryGenerator::Vertex v;
		v.Position.x = mesh->mVertices[i].x;
		v.Position.y = mesh->mVertices[i].y;
		v.Position.z = mesh->mVertices[i].z;
		v.Normal.x = mesh->mNormals[i].x;
		v.Normal.y = mesh->mNormals[i].y;
		v.Normal.z = mesh->mNormals[i].z;
		if (mesh->mTangents != nullptr)
		{
			v.TangentU.x = mesh->mTangents[i].x;
			v.TangentU.y = mesh->mTangents[i].y;
			v.TangentU.z = mesh->mTangents[i].z; 
			IsTangent = false;
		}


		if (mesh->mTextureCoords[0])
		{
			v.TexC.x = (float)mesh->mTextureCoords[0][i].x;
			v.TexC.y = (float)mesh->mTextureCoords[0][i].y;
		}
		meshData->Vertices.push_back(v);
	}

	// ¿Œµ¶Ω∫
	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; ++j)
		{
			meshData->Indices32.push_back(m_FaceOffset + face.mIndices[j]);
		}
	}
	m_FaceOffset = mesh->mNumVertices;
}