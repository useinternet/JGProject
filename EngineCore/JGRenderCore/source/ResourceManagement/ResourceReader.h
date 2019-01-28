#pragma once
#include"DxCommon/DxCommon.h"


namespace JGRC
{
	struct JGBoneData;
	struct JGBoneNode;
	class JGAnimation;
	class RCORE_EXPORT ResourceReader
	{
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
		void ReadSkeletalMesh(const std::string& path, GeometryGenerator::SkinnedMeshData& MeshData, std::vector<JGBoneData>& BoneData, JGBoneNode* Root);
		void ReadAnimation(const std::string& path, JGAnimation& Anim);
	};
}