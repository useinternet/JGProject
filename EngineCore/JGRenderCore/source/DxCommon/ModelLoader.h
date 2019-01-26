#pragma once
#include"DxCommon.h"
#include"GeometryGenerator.h"


struct aiNode;
struct aiMesh;
struct aiScene;
struct aiBone;
struct aiAnimMesh;
struct aiAnimation;
struct aiVectorKey;
struct aiQuatKey;
struct aiNodeAnim;
namespace JGRC
{
	class RCORE_EXPORT ModelLoader
	{
	public:
		bool Success   = true;
		bool IsTangent = true;
	public:
		// 기본 스태틱 메시 로드
		ModelLoader(const std::string& path, 
			        std::vector<GeometryGenerator::MeshData>* MeshData,
			        std::vector<std::string>* MeshName);

		// 스켈레탈 메시 로드 (본 데이터)
		ModelLoader(const std::string& path,
			        std::vector<GeometryGenerator::SkinnedMeshData>*   MeshData,
			        std::unordered_map<std::string, std::vector<struct JGBoneData>>* bonedatas,
			        std::unordered_map<std::string, struct JGBoneNode*>* boneHierarchy,
			        std::vector<std::string>* MeshName);
		ModelLoader(const std::string& path, std::vector<class JGAnimation>* AnimArray);

	private:
		void ProcessNode(aiNode* node, 
			             const aiScene* scene, 
			             std::vector<GeometryGenerator::MeshData>* MeshData, 
			             std::vector<std::string>* MeshName);
		void ProcessMesh(aiMesh* mesh, 
			             const aiScene* scene,
			             GeometryGenerator::MeshData* meshData);

		void ProcessNode(aiNode* node, 
			             const aiScene* scene, 
			             std::vector<GeometryGenerator::SkinnedMeshData>*   MeshData,
			             std::unordered_map<std::string, std::vector<struct JGBoneData>>* bonedatas,
			             std::vector<std::string>* MeshName);
		void ProcessMesh(aiMesh* mesh,
			             const aiScene* scene, 
			             GeometryGenerator::SkinnedMeshData* meshData,
			             std::vector<struct JGBoneData>* boneData);
		void ProcessBoneHierarchy(aiNode* node,
			                      struct JGBoneNode* boneNode,
			                      std::vector<struct JGBoneData>* bonedatas);
		void ProcessAnimation(aiAnimation* Anim, class JGAnimation* OutputAnim);

		void AddBoneData(UINT id, float w, UINT index, GeometryGenerator::SkinnedMeshData* meshData);
	};
}