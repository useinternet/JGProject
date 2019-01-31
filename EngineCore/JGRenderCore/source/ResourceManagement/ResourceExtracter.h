#pragma once
#include"DxCommon/DxCommon.h"


namespace JGRC
{
	struct RCORE_EXPORT JGVectorFrame;
	struct RCORE_EXPORT JGQuatFrame;
	struct RCORE_EXPORT JGAnimChannel;
	struct RCORE_EXPORT JGBoneData;
	struct RCORE_EXPORT JGBoneNode;
	class  RCORE_EXPORT JGAnimation;
	static const std::string global_static_mesh_file_format = ".jgmesh";
	static const std::string global_static_skeletal_mesh_file_format = ".jgskeletalmesh";
	static const std::string global_static_animation_file_format = ".jganimation";
	enum RCORE_EXPORT EExtracter_ResourceType
	{
		Resource_Extracter_Texture      = 0x001,
		Resource_Extracter_Animation    = 0x002,
		Resource_Extracter_SkeletalMesh = 0x004,
		Resource_Extracter_StaticMesh   = 0x008,
		Resource_Extracter_Material     = 0x016

	
	};
	inline EExtracter_ResourceType operator|(EExtracter_ResourceType bit1, EExtracter_ResourceType bit2)
	{
		return (EExtracter_ResourceType)((int)bit1 | (int)bit2);
	}
	class RCORE_EXPORT ResourceExtracter
	{
	private:
		std::string m_OutputPath;
		std::vector<std::string>                        m_MeshName;
		std::vector<GeometryGenerator::MeshData>        m_MeshData;
		std::vector<std::string>                        m_SkinnedMeshName;
		std::vector<GeometryGenerator::SkinnedMeshData> m_SkinnedMeshData;


		std::vector<std::string> m_Name;
		std::unordered_map<std::string, std::vector<JGBoneData>>  m_BoneData;
		std::unordered_map<std::string, JGBoneNode*>              m_BoneHierarchy;
		std::vector<JGAnimation>   m_Animation;
	public:
		bool StaticMeshSuccess = false;
		bool SkeletalMeshSuccess = false;
		bool AnimationSuccess = false;
	public:
		ResourceExtracter(EExtracter_ResourceType type, const std::string& outputPath, const std::string& path);
	private:
		void WriteStaticMeshData();
		void WriteSkeletalMeshData();
		void WriteAnimation();

	private:
		void WriteBoneHierarchy(std::ofstream& fout,JGBoneNode* Node);
	};
}