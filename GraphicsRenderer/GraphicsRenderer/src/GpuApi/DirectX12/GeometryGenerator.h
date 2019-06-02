#pragma once


#include"VertexCollection.h"
#include<vector>

namespace GR
{
	namespace Dx12
	{
		class GeometryGenerator
		{
		public:
			struct MeshData
			{
				std::vector<Vertex>   Vertices;
				std::vector<uint32_t> Indices;

				DirectX::BoundingBox  BBox;


				const uint32_t GetIndexCount() const {
					return (uint32_t)Indices.size();
				}

			};



		public:
			static MeshData CreateBox(float width, float height, float depth, UINT32 numSubdivisions);
			static MeshData CreateSphere(float radius, UINT32 sliceCount, UINT32 stackCount);
			static MeshData CreateGeosphere(float radius, UINT32 numSubdivisions);
			static MeshData CreateCylinder(float bottomRadius, float topRadius, float height, UINT32 sliceCount, UINT32 stackCount);
			static MeshData CreateGrid(float width, float depth, UINT32 m, UINT32 n);
			static MeshData CreateQuad(float x, float y, float w, float h, float depth);


		private:
			static void Subdivide(MeshData& meshData);
			static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
			static void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT32 sliceCount, UINT32 stackCount, MeshData& meshData);
			static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT32 sliceCount, UINT32 stackCount, MeshData& meshData);

		};

	}
}