#include "pch.h"
#include "ReMesh.h"
#include "Object/DxObject/CommandList.h"
using namespace std;

namespace RE
{
	void CreateBox(ReStaticMesh* mesh, float  width, float height, float depth);
	void ReMesh::Draw(CommandList* cmdList, uint32_t instanceCount)
	{
		cmdList->BindDynamicVertexBuffer(0, m_VertexTypeSize, m_MeshVertexDatas);
		cmdList->BindDynamicIndexBuffer(m_MeshIndexDatas);
		cmdList->SetPrimitiveTopology(m_Topology);
		cmdList->DrawIndexed((uint32_t)m_MeshIndexDatas.size(), instanceCount);
	}

	std::shared_ptr<ReMesh> ReMesh::Create(EReMeshShape shape)
	{
		std::shared_ptr<ReStaticMesh> result = make_shared<ReStaticMesh>();
		switch (shape)
		{
		case EReMeshShape::Box:
			CreateBox(result.get(), 1, 1, 1);
			break;

		}

		return result;
	}

	std::shared_ptr<ReGuiMesh> ReGuiMesh::CreateFillRect(float width, float height)
	{
		auto mesh = make_shared<ReGuiMesh>();

		std::vector<JGUIVertex> v(4);
		std::vector<uint32_t>   i(6);

		float hw = width * 0.5f;
		float hh = height * 0.5f;

		v[0] = JGUIVertex(-hw, -hh, 0, 0, 1);
		v[1] = JGUIVertex(-hw, +hh, 0, 0, 0);
		v[2] = JGUIVertex(+hw, +hh, 0, 1, 0);
		v[3] = JGUIVertex(+hw, -hh, 0, 1, 1);


		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 0; i[4] = 2; i[5] = 3;

		mesh->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mesh->Create(v, i);

		return mesh;
	}

	std::shared_ptr<ReGuiMesh> ReGuiMesh::CreateEmptyRect(float width, float height)
	{
		auto mesh = make_shared<ReGuiMesh>();

		std::vector<JGUIVertex> v(4);
		std::vector<uint32_t>   i(5);

		float hw = width * 0.5f;
		float hh = height * 0.5f;

		v[0] = JGUIVertex(-hw, -hh, 0, 0, 1);
		v[1] = JGUIVertex(-hw, +hh, 0, 0, 0);
		v[2] = JGUIVertex(+hw, +hh, 0, 1, 0);
		v[3] = JGUIVertex(+hw, -hh, 0, 1, 1);


		i[0] = 0; i[1] = 1; i[2] = 2; i[3] = 3; 
		i[4] = 0;

		mesh->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
		mesh->Create(v, i);
		return mesh;
	}


	void CreateBox(ReStaticMesh* mesh, float  width, float height, float depth)
	{
		std::vector<JVertex> v(24);

		float w2 = 0.5f * width;
		float h2 = 0.5f * height;
		float d2 = 0.5f * depth;

		// Fill in the front face vertex data.
		v[0] = JVertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[1] = JVertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[2] = JVertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[3] = JVertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the back face vertex data.
		v[4] = JVertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[5] = JVertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[6] = JVertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[7] = JVertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the top face vertex data.
		v[8] = JVertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[9] = JVertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[10] = JVertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[11] = JVertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the bottom face vertex data.
		v[12] = JVertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[13] = JVertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[14] = JVertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[15] = JVertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the left face vertex data.
		v[16] = JVertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[17] = JVertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[18] = JVertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[19] = JVertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// Fill in the right face vertex data.
		v[20] = JVertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[21] = JVertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[22] = JVertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[23] = JVertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);


		std::vector<uint32_t> i(36);

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

		mesh->Create(v, i);
	}



	


}
