#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"DirectXToolKit/VertexBuffer.h"
#include"DirectXToolKit/IndexBuffer.h"
#include"DirectXToolKit/CommandList.h"
namespace Dx12
{
	class GraphicsShader;
	typedef int EMeshType;
	namespace MeshType
	{
		enum
		{
			Static,
			Skeletal
		};
	}
	class Mesh
	{

	private:
		std::string  m_Name;
		EMeshType    m_MeshType;
		VertexBuffer m_VertexBuffer;
		IndexBuffer  m_IndexBuffer;


		D3D12_PRIMITIVE_TOPOLOGY m_TopologyType;
		uint32_t m_IndexCount;
	public:
		Mesh();
		~Mesh();

		template<typename Vertex, typename Index>
		void Inititalize(
			CommandList* commandList,
			const std::vector<Vertex>& vertics,
			const std::vector<Index>& indices,
			EMeshType type = MeshType::Static,
			const std::string& meshname = "Mesh") {

			m_MeshType = type;

			m_VertexBuffer.SetName(meshname + "_VertexBuffer");
			m_IndexBuffer.SetName(meshname + "_IndexBuffer");

			commandList->CopyBuffer(m_VertexBuffer, vertics);
			commandList->CopyBuffer(m_IndexBuffer, indices);

			m_IndexCount = static_cast<uint32_t>(indices.size());
			m_Name = move(meshname);
		}

		void RegisterShaderMacro(GraphicsShader& shader);
		void Draw(CommandList* commandList, uint32_t instanceCount = 1) const;


	public:
		EMeshType GetType() const {
			return m_MeshType;
		}
		uint32_t GetIndexCount() const {
			return m_IndexCount;
		}
		const std::string& GetName() const {
			return m_Name;
		}
		D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const {
			return m_TopologyType;
		}
	public:
		void SetName(const std::string& name) {
			m_Name = move(name);
		}
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY type) {
			m_TopologyType = type;
		}
	};
}