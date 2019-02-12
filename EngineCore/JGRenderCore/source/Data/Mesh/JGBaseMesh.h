#pragma once
#include"DxCommon/DxCommon.h"






namespace JGRC
{
	enum class EMeshType
	{
		Static,
		Skeletal
	};
	class RCORE_EXPORT JGBaseMesh
	{
		
		JGBaseMesh(const JGBaseMesh& copy) = delete;
		JGBaseMesh& operator=(const JGBaseMesh& copy) = delete;
	protected:
		std::string m_MeshName;
		EMeshType   m_MeshType;
		std::vector<std::string>      m_MeshArgNames;
		std::unique_ptr<MeshGeometry> m_MeshData;
		UINT m_CurrStartIndexLocation = 0;
		UINT m_CurrBaseVertexLocation = 0;
	public:
		JGBaseMesh() = default;
		virtual ~JGBaseMesh() = default;
		virtual void CreateMesh(ID3D12GraphicsCommandList* CommandList) {}
		virtual void ArgDraw(const std::string& name, ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, UINT Count = 1, D3D12_PRIMITIVE_TOPOLOGY TopolgyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			CommandList->IASetVertexBuffers(0, 1, &m_MeshData->VertexBufferView());
			CommandList->IASetIndexBuffer(&m_MeshData->IndexBufferView());
			CommandList->IASetPrimitiveTopology(TopolgyType);
			CommandList->DrawIndexedInstanced(
				m_MeshData->DrawArgs[name].IndexCount,
				Count,
				m_MeshData->DrawArgs[name].StartIndexLocation,
				m_MeshData->DrawArgs[name].BaseVertexLocation,
				0);
		}
	public:
		MeshGeometry* Data() { return m_MeshData.get(); }
		EMeshType     Type() { return m_MeshType; }
	public:
		const SubmeshGeometry& GetMeshArg_c(const std::string& name) const { return m_MeshData->DrawArgs[name]; }
		SubmeshGeometry&       GetMeshArg(const std::string& name)         { return m_MeshData->DrawArgs[name]; }
	public:
		const std::string& GetName() const                   { return m_MeshName; }
		const std::vector<std::string>& GetArgNames() const  { return m_MeshArgNames; }
		UINT  GetArgCount() const { return (UINT)m_MeshData->DrawArgs.size(); }
	};

	typedef GeometryGenerator::Vertex        Vertex;
	typedef std::vector<Vertex>              MeshVertex;
	typedef std::vector<std::uint32_t>       MeshIndex;
	typedef GeometryGenerator::SkinnedVertex SkeletalVertex;
	typedef std::vector<SkeletalVertex>      SkeletalMeshVertex;
	typedef std::vector<std::uint32_t>       SkeletalMeshIndex;
	struct RCORE_EXPORT JGBoneData
	{
		std::string Name;
		int        Index = -1;
		DirectX::XMFLOAT4X4 Offset = MathHelper::Identity4x4();
	};
	struct RCORE_EXPORT JGBoneNode
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
		JGBoneNode(const JGBoneNode& copy) = delete;
		JGBoneNode& operator=(const JGBoneNode& copy) = delete;
	};
}