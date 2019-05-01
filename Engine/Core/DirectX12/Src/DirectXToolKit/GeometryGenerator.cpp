#include<PCH.h>
#include"GeometryGenerator.h"
#include"VertexCollection.h"
#include"DirectXObjects/Mesh.h"
#include"CommandList.h"
#include<AssetManager.h>
using namespace std;
using namespace DirectX;
namespace Dx12
{
	namespace GeometryGenerator
	{
		void Subdivide(vector<JgVertex>& vertices, vector<uint32_t>& indices);
		JgVertex MidPoint(const JgVertex& v0, const JgVertex& v1);
		void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount,
			uint32_t stackCount, vector<JgVertex>& vertices, vector<uint32_t>& indices);
		void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount,
			uint32_t stackCount, vector<JgVertex>& vertices, vector<uint32_t>& indices);



		Mesh CreateMesh(CommandList* commandList, const string& path, bool* result)
		{
			fs::path ph(path);

			if (!fs::exists(ph))
			{
				if (result)
					*result = false;
				DX12_LOG_ERROR("%s is not exsit", path.c_str());
				return Mesh();
			}
			Mesh mesh;

			if (ph.extension() == ASSET_MESH_FILEFORMAT)
			{
				JgAsset::AssetMeshStream stream;
				bool r = false;
				r = JgAsset::AssetManager::LoadMesh(stream, path);

				if (result)
					*result = r;
				if (!r)
				{
					DX12_LOG_ERROR("Failed Load Mesh : %s ", path.c_str());
					return Mesh();
				}

				if (!stream.MeshData.IsTangents)
				{
					DX12_LOG_ERROR("%s is none tangent", path.c_str());
				}
				if (!stream.MeshData.IsTexcoords)
				{
					DX12_LOG_ERROR("%s is none texcoord", path.c_str());
				}
			

				mesh.Inititalize(commandList, stream.MeshData.AssetVertexs, stream.MeshData.AssetIndices, MeshType::Static, stream.MeshName);

				return mesh;
			}
			else
			{
				DX12_LOG_ERROR("unsupported file format");
				return Mesh();
			}
		}
		Mesh CreateSkeletalMesh(CommandList* commandList, const string& path, bool* result)
		{
			fs::path ph(path);

			if (!fs::exists(ph))
			{
				if (result)
					*result = false;
				DX12_LOG_ERROR("%s is not exsit", path.c_str());
				return Mesh();
			}

			Mesh mesh;

			if (ph.extension() == ASSET_SKELETALMESH_FILEFORMAT)
			{
				JgAsset::AssetSkeletalMeshStream stream;
				bool r = false;
				r = JgAsset::AssetManager::LoadSkeletalMesh(stream, path);

				if (result)
					*result = r;
				if (!r)
				{
					DX12_LOG_ERROR("Failed Load Mesh : %s ", path.c_str());
					return Mesh();
				}

				if (!stream.MeshData.IsTangents)
				{
					DX12_LOG_ERROR("%s is none tangent", path.c_str());
				}
				if (!stream.MeshData.IsTexcoords)
				{
					DX12_LOG_ERROR("%s is none texcoord", path.c_str());
				}


				mesh.Inititalize(commandList, stream.MeshData.AssetVertexs, stream.MeshData.AssetIndices, MeshType::Skeletal, stream.MeshName);

				return mesh;
			}
			else
			{
				DX12_LOG_ERROR("unsupported file format");
				return Mesh();
			}
		}
		Mesh CreateBox(
			CommandList* commandList,
			float width, float height, float depth,
			uint8_t numSubdivision)
		{
			std::vector<JgVertex> vertices;
			std::vector<uint32_t> indices;



			JgVertex v[24];

			float w2 = 0.5f*width;
			float h2 = 0.5f*height;
			float d2 = 0.5f*depth;

			// Fill in the front face vertex data.
			v[0] = JgVertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
			v[1] = JgVertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			v[2] = JgVertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
			v[3] = JgVertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

			// Fill in the back face vertex data.
			v[4] = JgVertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
			v[5] = JgVertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
			v[6] = JgVertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			v[7] = JgVertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

			// Fill in the top face vertex data.
			v[8] = JgVertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
			v[9] = JgVertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			v[10] = JgVertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
			v[11] = JgVertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

			// Fill in the bottom face vertex data.
			v[12] = JgVertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
			v[13] = JgVertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
			v[14] = JgVertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			v[15] = JgVertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

			// Fill in the left face vertex data.
			v[16] = JgVertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
			v[17] = JgVertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
			v[18] = JgVertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
			v[19] = JgVertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

			// Fill in the right face vertex data.
			v[20] = JgVertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
			v[21] = JgVertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
			v[22] = JgVertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
			v[23] = JgVertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

			vertices.assign(&v[0], &v[24]);

			//
			// Create the indices.
			//

			uint32_t i[36];

			// Fill in the front face index data
			i[0] = 0; i[1] = 1; i[2] = 2;
			i[3] = 0; i[4] = 2; i[5] = 3;

			// Fill in the back face index data
			i[6] = 4; i[7] = 5; i[8] = 6;
			i[9] = 4; i[10] = 6; i[11] = 7;

			// Fill in the top face index data
			i[12] = 8; i[13] = 9; i[14] = 10;
			i[15] = 8; i[16] = 10; i[17] = 11;

			// Fill in the bottom face index data
			i[18] = 12; i[19] = 13; i[20] = 14;
			i[21] = 12; i[22] = 14; i[23] = 15;

			// Fill in the left face index data
			i[24] = 16; i[25] = 17; i[26] = 18;
			i[27] = 16; i[28] = 18; i[29] = 19;

			// Fill in the right face index data
			i[30] = 20; i[31] = 21; i[32] = 22;
			i[33] = 20; i[34] = 22; i[35] = 23;

			indices.assign(&i[0], &i[36]);



			numSubdivision = std::min<uint32_t>(numSubdivision, 6u);

			for (uint32_t i = 0; i < numSubdivision; ++i)
			{
				Subdivide(vertices, indices);
			}


			Mesh mesh;
			mesh.Inititalize(commandList, vertices, indices, MeshType::Static, "Box");


			return mesh;
		}

		Mesh CreateSphere(
			CommandList* commandList,
			float radius, uint32_t sliceCount, uint32_t stackCount)
		{
			vector<JgVertex> vertices;
			vector<uint32_t> indices;



			JgVertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			JgVertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

			vertices.push_back(topVertex);

			float phiStep = JG_PI / stackCount;
			float thetaStep = 2.0f * JG_PI / sliceCount;

			for (uint32_t i = 1; i <= stackCount - 1; ++i)
			{
				float phi = i * phiStep;

				for (uint32_t j = 0; j <= sliceCount; ++j)
				{
					float theta = j * thetaStep;

					JgVertex v;

					// spherical to cartesian
					v.position.x = radius * sinf(phi)*cosf(theta);
					v.position.y = radius * cosf(phi);
					v.position.z = radius * sinf(phi)*sinf(theta);

					// Partial derivative of P with respect to theta
					v.tangent.x = -radius * sinf(phi)*sinf(theta);
					v.tangent.y = 0.0f;
					v.tangent.z = +radius * sinf(phi)*cosf(theta);

					XMVECTOR T = XMLoadFloat3(&v.tangent);
					XMStoreFloat3(&v.tangent, XMVector3Normalize(T));

					XMVECTOR p = XMLoadFloat3(&v.position);
					XMStoreFloat3(&v.normal, XMVector3Normalize(p));

					v.tex.x = theta / JG_2PI;
					v.tex.y = phi / JG_PI;

					vertices.push_back(v);
				}
			}

			vertices.push_back(bottomVertex);

			for (uint32_t i = 1; i <= sliceCount; ++i)
			{
				indices.push_back(0);
				indices.push_back(i + 1);
				indices.push_back(i);
			}


			uint32_t baseIndex = 1;
			uint32_t ringVertexCount = sliceCount + 1;
			for (uint32_t i = 0; i < stackCount - 2; ++i)
			{
				for (uint32_t j = 0; j < sliceCount; ++j)
				{
					indices.push_back(baseIndex + i * ringVertexCount + j);
					indices.push_back(baseIndex + i * ringVertexCount + j + 1);
					indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

					indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
					indices.push_back(baseIndex + i * ringVertexCount + j + 1);
					indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
				}
			}

			uint32_t southPoleIndex = (uint32_t)vertices.size() - 1;
			baseIndex = southPoleIndex - ringVertexCount;


			for (uint32_t i = 0; i < sliceCount; ++i)
			{
				indices.push_back(southPoleIndex);
				indices.push_back(baseIndex + i);
				indices.push_back(baseIndex + i + 1);
			}


			Mesh mesh;
			mesh.Inititalize(commandList, vertices, indices, MeshType::Static, "Sphere");

			return mesh;
		}
		Mesh CreateCylinder(
			CommandList* commandList,
			float bottomRadius, float topRadius, float height,
			uint32_t sliceCount, uint32_t stackCount)
		{
			vector<JgVertex> vertices;
			vector<uint32_t> indices;



			float stackHeight = height / stackCount;
			float radiusStep = (topRadius - bottomRadius) / stackCount;

			uint32_t ringCount = stackCount + 1;

			for (uint32_t i = 0; i < ringCount; ++i)
			{
				float y = -0.5f*height + i * stackHeight;
				float r = bottomRadius + i * radiusStep;

				float dTheta = 2.0f * JG_PI / sliceCount;
				for (uint32_t j = 0; j <= sliceCount; ++j)
				{
					JgVertex vertex;

					float c = cosf(j*dTheta);
					float s = sinf(j*dTheta);

					vertex.position = XMFLOAT3(r*c, y, r*s);

					vertex.tex.x = (float)j / sliceCount;
					vertex.tex.y = 1.0f - (float)i / stackCount;

					vertex.tangent = XMFLOAT3(-s, 0.0f, c);

					float dr = bottomRadius - topRadius;
					XMFLOAT3 bitangent(dr*c, -height, dr*s);

					XMVECTOR T = XMLoadFloat3(&vertex.tangent);
					XMVECTOR B = XMLoadFloat3(&bitangent);
					XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
					XMStoreFloat3(&vertex.normal, N);

					vertices.push_back(vertex);
				}
			}


			uint32_t ringVertexCount = sliceCount + 1;

		
			for (uint32_t i = 0; i < stackCount; ++i)
			{
				for (uint32_t j = 0; j < sliceCount; ++j)
				{
					indices.push_back(i*ringVertexCount + j);
					indices.push_back((i + 1)*ringVertexCount + j);
					indices.push_back((i + 1)*ringVertexCount + j + 1);

					indices.push_back(i*ringVertexCount + j);
					indices.push_back((i + 1)*ringVertexCount + j + 1);
					indices.push_back(i*ringVertexCount + j + 1);
				}
			}

			BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, vertices,indices);
			BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, vertices, indices);


			Mesh mesh;
			mesh.Inititalize(commandList, vertices, indices, MeshType::Static, "Cylinder");

			return mesh;
		}

		Mesh CreateGrid(
			CommandList* commandList,
			float width, float depth, uint32_t m, uint32_t n)
		{
			vector<JgVertex> vertices;
			vector<uint32_t> indices;

			uint32_t vertexCount = m * n;
			uint32_t faceCount = (m - 1)*(n - 1) * 2;


			float halfWidth = 0.5f*width;
			float halfDepth = 0.5f*depth;

			float dx = width / (n - 1);
			float dz = depth / (m - 1);

			float du = 1.0f / (n - 1);
			float dv = 1.0f / (m - 1);


			vertices.resize(vertexCount);

			for (uint32_t i = 0; i < m; ++i)
			{
				float z = halfDepth - i * dz;
				for (uint32_t j = 0; j < n; ++j)
				{
					float x = -halfWidth + j * dx;

					vertices[i*n + j].position = XMFLOAT3(x, 0.0f, z);
					vertices[i*n + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
					vertices[i*n + j].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

					vertices[i*n + j].tex.x = j * du;
					vertices[i*n + j].tex.y = i * dv;
				}
			}

			indices.resize(faceCount * 3);


			uint32_t k = 0;
			for (uint32_t i = 0; i < m - 1; ++i)
			{
				for (uint32_t j = 0; j < n - 1; ++j)
				{
					indices[k] = i * n + j;
					indices[k + 1] = i * n + j + 1;
					indices[k + 2] = (i + 1)*n + j;

					indices[k + 3] = (i + 1)*n + j;
					indices[k + 4] = i * n + j + 1;
					indices[k + 5] = (i + 1)*n + j + 1;

					k += 6;
				}
			}

			Mesh mesh;
			mesh.Inititalize(commandList, vertices, indices, MeshType::Static, "Grid");
			return mesh;
		}

		Mesh CreateQuad(
			CommandList* commandList,
			float x, float y, float w, float h, float depth)
		{
			vector<JgVertex> vertices(4);
			vector<uint32_t> indices(6);


			vertices[0] = JgVertex(
				x, y - h, depth,
				0.0f, 0.0f, -1.0f,
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f);

			vertices[1] = JgVertex(
				x, y, depth,
				0.0f, 0.0f, -1.0f,
				1.0f, 0.0f, 0.0f,
				0.0f, 0.0f);

			vertices[2] = JgVertex(
				x + w, y, depth,
				0.0f, 0.0f, -1.0f,
				1.0f, 0.0f, 0.0f,
				1.0f, 0.0f);

			vertices[3] = JgVertex(
				x + w, y - h, depth,
				0.0f, 0.0f, -1.0f,
				1.0f, 0.0f, 0.0f,
				1.0f, 1.0f);

			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;

			indices[3] = 0;
			indices[4] = 2;
			indices[5] = 3;

			Mesh mesh;
			mesh.Inititalize(commandList, vertices, indices, MeshType::Static, "Quad");

			return mesh;


		}














		void Subdivide(vector<JgVertex>& vertices, vector<uint32_t>& indices)
		{
			auto inputCopy_vertices = vertices;
			auto inputCopy_indices = indices;

			vertices.resize(0);
			indices.resize(0);


			uint32_t numTris = (uint32_t)inputCopy_indices.size() / 3;

			for (uint32_t i = 0; i < numTris; ++i)
			{
				JgVertex v0 = inputCopy_vertices[inputCopy_indices[i * 3 + 0]];
				JgVertex v1 = inputCopy_vertices[inputCopy_indices[i * 3 + 1]];
				JgVertex v2 = inputCopy_vertices[inputCopy_indices[i * 3 + 2]];

				JgVertex m0 = MidPoint(v0, v1);
				JgVertex m1 = MidPoint(v1, v2);
				JgVertex m2 = MidPoint(v0, v2);

				vertices.push_back(v0);
				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(m0);
				vertices.push_back(m1);
				vertices.push_back(m2);



				indices.push_back(i * 6 + 0);
				indices.push_back(i * 6 + 3);
				indices.push_back(i * 6 + 5);


				indices.push_back(i * 6 + 3);
				indices.push_back(i * 6 + 4);
				indices.push_back(i * 6 + 5);

				indices.push_back(i * 6 + 5);
				indices.push_back(i * 6 + 4);
				indices.push_back(i * 6 + 2);

				indices.push_back(i * 6 + 3);
				indices.push_back(i * 6 + 1);
				indices.push_back(i * 6 + 4);

			}
		}
		JgVertex MidPoint(const JgVertex& v0, const JgVertex& v1)
		{
			XMVECTOR p0 = XMLoadFloat3(&v0.position);
			XMVECTOR p1 = XMLoadFloat3(&v1.position);

			XMVECTOR n0 = XMLoadFloat3(&v0.normal);
			XMVECTOR n1 = XMLoadFloat3(&v1.normal);

			XMVECTOR tan0 = XMLoadFloat3(&v0.tangent);
			XMVECTOR tan1 = XMLoadFloat3(&v1.tangent);

			XMVECTOR tex0 = XMLoadFloat2(&v0.tex);
			XMVECTOR tex1 = XMLoadFloat2(&v1.tex);


			XMVECTOR pos = 0.5f * (p0 + p1);
			XMVECTOR normal = XMVector3Normalize(0.5f * (n0 + n1));
			XMVECTOR tangent = XMVector3Normalize(0.5f * (tan0 + tan1));
			XMVECTOR tex = 0.5f * (tex0 + tex1);

			JgVertex v;
			XMStoreFloat3(&v.position, pos);
			XMStoreFloat3(&v.normal, normal);
			XMStoreFloat3(&v.tangent, tangent);
			XMStoreFloat2(&v.tex, tex);

			return v;

		}
		void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount,
			uint32_t stackCount, vector<JgVertex>& vertices, vector<uint32_t>& indices)
		{
			uint32_t baseIndex = (uint32_t)vertices.size();

			float y = 0.5f * height;
			float dTheta = 2.0f * PI / sliceCount;

			for (uint32_t i = 0; i <= sliceCount; ++i)
			{
				float x = topRadius * cosf(i*dTheta);
				float z = topRadius * sinf(i*dTheta);

				float u = x / height + 0.5f;
				float v = z / height + 0.5f;

				vertices.push_back(JgVertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
			}

			vertices.push_back(JgVertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

			uint32_t centerIndex = (uint32_t)vertices.size() - 1;

			for (uint32_t i = 0; i < sliceCount; ++i)
			{
				indices.push_back(centerIndex);
				indices.push_back(baseIndex + i + 1);
				indices.push_back(baseIndex + i);
			}
		}
		void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount,
			uint32_t stackCount, vector<JgVertex>& vertices, vector<uint32_t>& indices)
		{
			uint32_t baseIndex = (uint32_t)vertices.size();
			float y = -0.5f*height;


			float dTheta = 2.0f*XM_PI / sliceCount;
			for (uint32_t i = 0; i <= sliceCount; ++i)
			{
				float x = bottomRadius * cosf(i*dTheta);
				float z = bottomRadius * sinf(i*dTheta);

	
				float u = x / height + 0.5f;
				float v = z / height + 0.5f;

				vertices.push_back(JgVertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
			}

			// Cap center vertex.
			vertices.push_back(JgVertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

			// Cache the index of center vertex.
			uint32_t centerIndex = (uint32_t)vertices.size() - 1;

			for (uint32_t i = 0; i < sliceCount; ++i)
			{
				indices.push_back(centerIndex);
				indices.push_back(baseIndex + i);
				indices.push_back(baseIndex + i + 1);
			}


		}
	}
}