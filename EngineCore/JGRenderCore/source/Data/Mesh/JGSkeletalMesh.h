#pragma once
#include"JGBaseMesh.h"


namespace JGRC
{
	typedef GeometryGenerator::SkinnedVertex SkeletalVertex;
	typedef std::vector<SkeletalVertex>       SkeletalMeshVertex;
	typedef std::vector<std::uint32_t>       SkeletalMeshIndex;
	struct JGBoneData
	{
		std::string Name;
		int        Index = -1;
		DirectX::XMFLOAT4X4 Offset = MathHelper::Identity4x4();
	};
	struct JGBoneNode
	{
		// Static 메모리 할당에도 불구하고 시스템 종료전에 할당이 해제된다.. 이유가 뭘까..
	private:
		static std::vector<std::unique_ptr<JGBoneNode>> BoneMemNodes;
	public:
		static JGBoneNode* CreateNode()
		{
			auto node = std::make_unique<JGBoneNode>();
			JGBoneNode* result = node.get();
			BoneMemNodes.push_back(move(node));
			return result;
		}
	public:
		JGBoneData  Data;
		int        NodeIndex = -1;
		JGBoneNode* Parent = nullptr;
		std::vector<JGBoneNode*> Child;
		bool bExist = false;
		DirectX::XMFLOAT4X4 Transform = MathHelper::Identity4x4();
	public:
		JGBoneNode() = default;
		JGBoneNode(const JGBoneNode& copy)            = delete;
		JGBoneNode& operator=(const JGBoneNode& copy) = delete;
	};
	class RCORE_EXPORT JGSkeletalMesh : public JGBaseMesh
	{
		static UINT SkinnedIndex;
		typedef std::vector<JGBoneData> BoneArray;
	private:
		SkeletalMeshVertex m_Vertex;
		SkeletalMeshIndex  m_Index;
		//
		std::unordered_map<std::string, BoneArray>     m_BoneDatas;
		std::unordered_map<std::string, JGBoneNode*>   m_BoneHierarchy;
		//
		std::unordered_map<std::string, class JGAnimationHelper> m_AnimtionHelpers;
		std::unordered_map<std::string, UINT> m_SkinnedCBIndex;
		class JGAnimation* m_SkeletalAnim = nullptr;
	public:
		JGSkeletalMesh(const std::string& name);
		virtual void CreateMesh(ID3D12GraphicsCommandList* CommandList) override;
		virtual void Update(const GameTimer& gt, FrameResource* CurrFrameResource,const std::string& name) override;
		virtual void ArgDraw(
			const std::string& name,
			ID3D12GraphicsCommandList* CommandList,
			FrameResource* CurrFrameResource,
			UINT Count = 1,
			D3D12_PRIMITIVE_TOPOLOGY TopolgyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override;
		virtual void Draw(
			ID3D12GraphicsCommandList* CommandList,
			FrameResource* CurrFrameResource,
			UINT Count = 1,
			D3D12_PRIMITIVE_TOPOLOGY TopolgyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override;
		void SetAnimation(const std::string& name);
	public:
		void AddFbxMeshArg(const std::string& path);
		void AddSkeletalMeshArg(const std::string& path);
	private:
		void AddMeshArg(const std::string& name, const SkeletalMeshVertex& vertex, const SkeletalMeshIndex& index);
	};


}