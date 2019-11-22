#pragma once


#include "Object/ReObject.h"
#include <vector>
namespace RE
{
	class CommandList;
	enum class EReMeshType
	{
		Static,
		Skeletal,
		GUI
	};
	enum class EReMeshShape
	{
		Box
	};
	class ReMesh : public ReObject
	{

	protected:
		using VertexData = std::vector<byte>;
		using IndexData  = std::vector<uint32_t>;
	public:
		ReMesh(EReMeshType type) : ReObject("ReMesh"), m_MeshType(type) {}
		ReMesh(const std::string& name, EReMeshType type) : ReObject(name), m_MeshType(type) {}
	public:


		uint32_t GetTypeSize() const {
			return m_VertexTypeSize;
		}
		EReMeshType GetType() const {
			return m_MeshType;
		}
		const VertexData& GetVertexData() const {
			return m_MeshVertexDatas;
		}
		const IndexData& GetIndexData() const {
			return m_MeshIndexDatas;
		}
		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology) {
			m_Topology = topology;
		}
		void Draw(CommandList* cmdList, uint32_t instanceCount = 1);
	public:
		static std::shared_ptr<ReMesh> Create(EReMeshShape shape);
	protected:
		template<typename T>
		void Set(const std::vector<T>& v, const IndexData& i)
		{
			m_VertexTypeSize = (uint32_t)sizeof(T);
			size_t btSize = v.size() * sizeof(T);
			m_MeshVertexDatas.resize(btSize);
			memcpy_s(&m_MeshVertexDatas[0], btSize, v.data(), btSize);
			m_MeshIndexDatas = std::move(i);
		}

		EReMeshType  m_MeshType;
		VertexData   m_MeshVertexDatas;
		IndexData    m_MeshIndexDatas;
		uint32_t     m_VertexTypeSize;
		D3D_PRIMITIVE_TOPOLOGY m_Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	public:
		static ReMesh Merge(const ReMesh& m1, const ReMesh& m2);
	};


	class ReStaticMesh : public ReMesh
	{
	public:
		ReStaticMesh() : ReMesh("ReStaticMesh", EReMeshType::Static) {}
		ReStaticMesh(const std::string& name) : ReMesh(name, EReMeshType::Static) {}

		void Create(const std::vector<JVertex>& v, const IndexData& i)
		{
			Set(v, i);
		}
	};

	class ReSkeletalMesh : public ReMesh
	{
	public:
		ReSkeletalMesh() : ReMesh("ReSkeletalMesh", EReMeshType::Skeletal) {}
		ReSkeletalMesh(const std::string& name) : ReMesh(name, EReMeshType::Skeletal) {}

		void Create(const std::vector<JSkinnedVertex>& v, const IndexData& i)
		{
			Set(v, i);
		}
	};

	class ReGuiMesh : public ReMesh
	{
	public:
		ReGuiMesh() : ReMesh("ReGuiMesh", EReMeshType::GUI) {}
		ReGuiMesh(const std::string& name) : ReMesh(name, EReMeshType::GUI) {}
	};


}