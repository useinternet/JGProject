#pragma once
#pragma once
#include"AssetDefine.h"

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiBone;
struct aiAnimMesh;
struct aiAnimation;
struct aiVectorKey;
struct aiQuatKey;
struct aiNodeAnim;
namespace JgAsset
{
	class AssetExtracter
	{
	private:
		uint32_t m_BoneIndex = 0;
		std::unordered_map<aiNode*, AssetBoneNode>    m_AssetBoneNodeByAi;
	public:
		bool Extract(const std::string& path, const std::string& outputpath, EAssetTypes type);
	private:
		void LoadMesh(aiNode* node, const aiScene* scene, AssetMeshPackage& Meshpackage);
		void OutputFile_Mesh(const std::string& outputpath, AssetMeshPackage& Meshpackage);


		void LoadBoneHierarchy(aiNode* node, aiNode* parent, const aiScene* scene);
		void OutputFile_BoneHierarchy(const std::string& outputPath, AssetBoneHierarchyPackage& bonePackage);


		void LoadSkeletalMesh(aiNode* node, const aiScene* scene,
			AssetSkeletalMeshPackage& Meshpackage);
		void OutputFile_SkMesh(const std::string& outputpath, AssetSkeletalMeshPackage& Meshpackage);


		void LoadAnimation(aiAnimation* anim, AssetAnimation& output);
		void OutputFile_Animation(const std::string& outputPath, AssetAnimation& animation);
	};
}