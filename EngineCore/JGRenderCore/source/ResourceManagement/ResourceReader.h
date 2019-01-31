#pragma once
#include"DxCommon/DxCommon.h"


namespace JGRC
{
	struct JGBoneData;
	struct JGBoneNode;
	class JGAnimation;
	class RCORE_EXPORT ResourceReader
	{
		static std::unordered_map<std::string, GeometryGenerator::MeshData> MeshDatas;
		static std::unordered_map<std::string, GeometryGenerator::SkinnedMeshData> SkinnedMeshDatas;
		static std::unordered_map<std::string, std::vector<JGBoneData>> BoneDatas;
		static std::unordered_map<std::string, JGBoneNode*> BoneRoots;
		static std::unordered_map<std::string, JGAnimation> Animations;
	public:
		bool Success = false;
	public:
		ResourceReader(
			const std::string& path,
			GeometryGenerator::MeshData& MeshData);
		ResourceReader(
			const std::string& path, 
			GeometryGenerator::SkinnedMeshData& MeshData,
			std::vector<JGBoneData>& BoneData,
			JGBoneNode** Root);
		ResourceReader(
			const std::string& path,
			class JGAnimation& Anim);
	private:
		void ReadStaticMesh(const std::string& path, GeometryGenerator::MeshData& MeshData);
		void ReadSkeletalMesh(const std::string& path, GeometryGenerator::SkinnedMeshData& MeshData, std::vector<JGBoneData>& BoneData, JGBoneNode* Root);
		void ReadAnimation(const std::string& path, JGAnimation& Anim);
	};
}