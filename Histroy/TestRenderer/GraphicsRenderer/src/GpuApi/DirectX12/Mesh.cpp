#include"pch.h"
#include"Mesh.h"
#include"Commander.h"
using namespace std;

namespace GR
{
	namespace Dx12
	{
		D3D12_PRIMITIVE_TOPOLOGY ConvertTopology(Mesh::EMeshTopology topology);
		Mesh::Mesh() :
			m_MeshType(STATIC),
			m_IndexLocationOffset(0),
			m_VertexLocationOffset(0),
			m_PrimitiveTopology(TRIANGLE_LIST) {}

		void Mesh::SetPrimitiveTopology(EMeshTopology type)
		{
			m_PrimitiveTopology = type;
		}
		void Mesh::SetMeshType(EMeshType type)
		{
			assert((!m_VertexBuffer.IsValid() || !m_IndexBuffer.IsValid()) && "Mesh Type Set Before Add subMesh.");
			m_MeshType = type;
		}
		void Mesh::CreateMesh(Commander* commander, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		{
			assert(m_MeshType == STATIC && "Current this mesh is skeletalMesh..!! but Adding Mesh is Static Mesh..");
			// Vertex 데이터 버퍼 복사
			uint32_t numVertex = (uint32_t)vertices.size();
			const Vertex*  vertexDatas = vertices.data();

			commander->CopyBuffer(
				m_VertexBuffer, numVertex, sizeof(Vertex), vertexDatas);


			// Index 데이터 버퍼 복사
			uint32_t numIndex = (uint32_t)indices.size();
			const uint32_t* indexDatas = indices.data();

			commander->CopyBuffer(
				m_IndexBuffer, numIndex, sizeof(uint32_t), indexDatas);

			// 리소스 장벽 전환
			commander->TransitionBarrier(m_VertexBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			commander->TransitionBarrier(m_IndexBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);
			commander->FlushResourceBarriers();

			// 서브 메시 저장
			SubMesh submesh;
			submesh.StartIndexLocation = m_IndexLocationOffset;
			submesh.BaseVertexLocation = m_VertexLocationOffset;
			submesh.IndexCount = numIndex;
			submesh.VertexCount = numVertex;
			m_DrawArgs[""] = move(submesh);

			// 
			m_IndexLocationOffset += numIndex;
			m_VertexLocationOffset += numVertex;
			//
			DirectX::BoundingBox::CreateFromPoints(m_BoundingBox, numVertex, &vertices[0].position, sizeof(Vertex));
		}
		void Mesh::CreateMesh(Commander* commander,const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t>& indices)
		{
			assert(m_MeshType == SKELETAL && "Current this mesh is static Mesh..!! but Adding Mesh is Skeletal Mesh..");
			// Vertex 데이터 버퍼 복사
			uint32_t numVertex = (uint32_t)vertices.size();
			const SkinnedVertex*  vertexDatas = vertices.data();

			commander->CopyBuffer(
				m_VertexBuffer, numVertex, sizeof(SkinnedVertex), vertexDatas);


			// Index 데이터 버퍼 복사
			uint32_t numIndex = (uint32_t)indices.size();
			const uint32_t* indexDatas = indices.data();

			commander->CopyBuffer(
				m_IndexBuffer, numIndex, sizeof(uint32_t), indexDatas);


			commander->TransitionBarrier(m_VertexBuffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			commander->TransitionBarrier(m_IndexBuffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);
			commander->FlushResourceBarriers();


			SubMesh submesh;
			submesh.StartIndexLocation = m_IndexLocationOffset;
			submesh.BaseVertexLocation = m_VertexLocationOffset;
			submesh.IndexCount = numIndex;
			submesh.VertexCount = numVertex;
			m_DrawArgs[""] = move(submesh);

			m_IndexLocationOffset += numIndex;
			m_VertexLocationOffset += numVertex;

			DirectX::BoundingBox::CreateFromPoints(m_BoundingBox, numVertex, &vertices[0].position, sizeof(SkinnedVertex));
		}
		void Mesh::SetIgnoreSubMesh(const std::vector<std::string>& meshID, bool isIgnore)
		{
			for (auto& id : meshID)
			{
				if (m_DrawArgs.find(id) != m_DrawArgs.end())
				{
					m_DrawArgs[id].IsIgnore = isIgnore;
				}
			}
		}
		void Mesh::Partion(const std::string& meshID, uint32_t startVertexLocation, uint32_t endVertexLocation,
			uint32_t startIndexLocation, uint32_t endIndexLocation)
		{
			// 일단 DrawArgs중에 어디 Submesh 에 속해있는지 검사
			std::string belong_to_meshID;
			SubMesh belong_to_submesh;
			uint32_t vstart;
			uint32_t vend;
			uint32_t istart;
			uint32_t iend;
			for (auto& submesh : m_DrawArgs)
			{
				vstart = submesh.second.BaseVertexLocation;
				vend = submesh.second.BaseVertexLocation + submesh.second.VertexCount;
				istart = submesh.second.StartIndexLocation;
				iend = submesh.second.StartIndexLocation + submesh.second.IndexCount;
				if (vstart <= startVertexLocation && endVertexLocation <= vend &&
					istart <= startIndexLocation && endIndexLocation <= iend)
				{
					belong_to_meshID = submesh.first;
					belong_to_submesh = submesh.second;
					break;
				}
			}
			bool left = startVertexLocation == vstart && startIndexLocation == istart;
			bool right = endVertexLocation == vend && endIndexLocation == iend;


			if (left && right)
			{
				m_DrawArgs.erase(belong_to_meshID);
				m_DrawArgs[meshID] = belong_to_submesh;
				
			}
			else if (left && !right)
			{
				SubMesh arg;
				arg.BaseVertexLocation = vstart;
				arg.StartIndexLocation = istart;
				arg.IndexCount = endIndexLocation - startIndexLocation;
				arg.VertexCount = endVertexLocation - startVertexLocation;

				SubMesh arg2;
				arg2.BaseVertexLocation = vstart + arg.VertexCount;
				arg2.StartIndexLocation = istart + arg.IndexCount;
				arg2.IndexCount  = belong_to_submesh.IndexCount - arg.IndexCount;
				arg2.VertexCount = belong_to_submesh.VertexCount - arg.VertexCount;

				m_DrawArgs[meshID] = arg;
				m_DrawArgs[belong_to_meshID] = arg2;
			}
			else if (!left && right)
			{
				// 
				
				SubMesh arg;
				arg.IndexCount = endIndexLocation - startIndexLocation;
				arg.VertexCount = endVertexLocation - startVertexLocation;
				arg.BaseVertexLocation = vstart + arg.VertexCount;
				arg.StartIndexLocation = istart + arg.IndexCount;

				SubMesh arg2;
				arg2.BaseVertexLocation = vstart;
				arg2.StartIndexLocation = istart;
				arg2.IndexCount = belong_to_submesh.IndexCount - arg.IndexCount;
				arg2.VertexCount = belong_to_submesh.VertexCount - arg.VertexCount;

				
				m_DrawArgs[meshID] = arg;
				m_DrawArgs[belong_to_meshID] = arg2;
			}
			else if (!left && !right)
			{
				SubMesh arg;
				arg.BaseVertexLocation = vstart;
				arg.StartIndexLocation = istart;
				arg.IndexCount = startIndexLocation - istart;
				arg.VertexCount = startVertexLocation - vstart;

				SubMesh arg2;
				arg2.BaseVertexLocation = startVertexLocation;
				arg2.StartIndexLocation = startIndexLocation;
				arg2.IndexCount = endIndexLocation - startIndexLocation;
				arg2.VertexCount = endVertexLocation - startVertexLocation;


				SubMesh arg3;
				arg3.BaseVertexLocation = startVertexLocation + arg2.VertexCount;
				arg3.StartIndexLocation = startIndexLocation + arg2.IndexCount;
				arg3.IndexCount = belong_to_submesh.IndexCount - arg.IndexCount - arg2.IndexCount;
				arg3.VertexCount = belong_to_submesh.VertexCount - arg.VertexCount - arg2.VertexCount;

				m_DrawArgs[belong_to_meshID + "1"] = arg;
				m_DrawArgs[meshID] = arg2;
				m_DrawArgs[belong_to_meshID + "2"] = arg3;
				m_DrawArgs.erase(belong_to_meshID);
			}

		}
		void Mesh::Draw(GraphicsCommander* commander, uint32_t instanceCount)
		{
			commander->SetVertexBuffer(m_VertexBuffer);
			commander->SetIndexBuffer(m_IndexBuffer);
			commander->SetPrimitiveTopology(ConvertTopology(m_PrimitiveTopology));

			for (auto& arg : m_DrawArgs)
			{
				if (arg.second.IsIgnore)
					continue;
				commander->DrawIndexed(arg.second.IndexCount, instanceCount,
					arg.second.StartIndexLocation, arg.second.BaseVertexLocation);
			}

		}

		SubMesh* Mesh::GetSubMesh(const std::string& meshID)
		{
			if (m_DrawArgs.find(meshID) == m_DrawArgs.end())
			{
				return nullptr;
			}


			return &m_DrawArgs[meshID];
		}
		const DirectX::BoundingBox& Mesh::GetBoundingBox() const
		{
			return m_BoundingBox;
		}

		D3D12_PRIMITIVE_TOPOLOGY ConvertTopology(Mesh::EMeshTopology topology)
		{
			switch (topology)
			{
			case Mesh::UNDEFINED:
				return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
			case Mesh::POINT_LIST:
				return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			case Mesh::LINE_LIST:
				return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			case Mesh::LINE_STRIP:
				return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			case Mesh::TRIANGLE_LIST:
				return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case Mesh::TRIANGLE_STRIP:
				return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			default:
				return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
			}
		}
	}
}