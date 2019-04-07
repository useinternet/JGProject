#pragma once
#include"AssetDefine.h"


namespace JgAsset
{
	namespace AssetManager
	{
		// 로드
		bool LoadMesh(AssetMeshStream& stream, const std::string& path);
		bool LoadSkeletalMesh(AssetSkeletalMeshStream& stream, const std::string& path);
		bool LoadSkeletal(AssetBoneHierarchyStream& stream, const std::string& path);
		bool LoadAnimation(AssetAnimationStream& stream, const std::string& path);


		// 생성
		bool CreateAnimationTransformsByTimePos(
			AssetAnimTransformByTimePos& Output,
			const std::string& skMeshPath,
			const std::string& animPath, 
			const std::string& skeletalPath);


	}
}