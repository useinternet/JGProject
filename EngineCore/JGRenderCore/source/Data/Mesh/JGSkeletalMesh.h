#pragma once
#include"JGBaseMesh.h"


namespace JGRC
{

	class RCORE_EXPORT JGSkeletalMesh : public JGBaseMesh
	{
		typedef std::vector<JGBoneData> BoneArray;
	private:
		SkeletalMeshVertex m_Vertex;
		SkeletalMeshIndex  m_Index;
		//
		std::unordered_map<std::string, BoneArray>     m_BoneDatas;
		std::unordered_map<std::string, JGBoneNode*>   m_BoneHierarchy;
	public:
		JGSkeletalMesh(const std::string& name);
		virtual void CreateMesh(ID3D12GraphicsCommandList* CommandList) override;
	public:
		BoneArray&   GetBoneData(const std::string& name);
		JGBoneNode*  GetBoneHierarchy(const std::string& name);
	public:
		void AddSkeletalMeshArg(const std::string& path);
	private:
		void AddMeshArg(const std::string& name, const SkeletalMeshVertex& vertex, const SkeletalMeshIndex& index);
	};


}