#pragma once
#include"Dx12Include.h"
#include"GraphicsResource.h"
#include"VertexCollection.h"
namespace GR
{
	namespace Dx12
	{
		class Commander;
		class GraphicsCommander;
	
		struct SubMesh
		{
			uint32_t IndexCount = 0;
			uint32_t VertexCount = 0;
			uint32_t StartIndexLocation = 0;
			uint32_t BaseVertexLocation = 0;
			bool     IsIgnore = false;
		};
		class Mesh
		{
		public:
			enum EMeshTopology
			{
				UNDEFINED,
				POINT_LIST,
				LINE_LIST,
				LINE_STRIP,
				TRIANGLE_LIST,
				TRIANGLE_STRIP
			};
			enum EMeshType
			{
				STATIC,
				SKELETAL
			};
		private:
			VertexBuffer m_VertexBuffer;
			IndexBuffer  m_IndexBuffer;
			EMeshTopology m_PrimitiveTopology;

			EMeshType m_MeshType;
			DirectX::BoundingBox m_BoundingBox;

			// SubMesh ฐüทร
			std::map<std::string, SubMesh> m_DrawArgs;
			uint32_t m_IndexLocationOffset;
			uint32_t m_VertexLocationOffset;
		public:
			Mesh();
			void SetPrimitiveTopology(EMeshTopology type);
			void SetMeshType(EMeshType type);
			void CreateMesh(Commander* commander,const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
			void CreateMesh(Commander* commander, const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t>& indices);
			void SetIgnoreSubMesh(const std::vector<std::string>& meshID, bool isIgnore);
			void Partion(const std::string& meshID, uint32_t startVertexLocation, uint32_t endVertexLocation,
				uint32_t startIndexLocation, uint32_t endIndexLocation);
			void Draw(GraphicsCommander* commander, uint32_t instanceCount = 1);

			SubMesh* GetSubMesh(const std::string& meshID);
			const DirectX::BoundingBox& GetBoundingBox() const;
		};
	}
}