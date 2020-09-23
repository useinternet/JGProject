#pragma once
#include "Data/SDCollection.h"
#include "Math/JBBox.h"
#include <vector>
#include <string>




class MeshGenerator
{
public:
	struct StaticMeshData
	{
		std::vector<StaticVertex> v;
		std::vector<uint32_t>     i;
		JBBox bbox;
	};
	static bool LoadFromFile(const std::wstring& path, std::vector<StaticMeshData>& out_meshDatas);
	// PaperVertex
	static void Quad(float width, float height, float z,  std::vector<PaperVertex>& out_v, std::vector<uint32_t>& out_i)
	{
		float hw = width * 0.5f;
		float hh = height * 0.5f;

		out_v.resize(4);
		out_v[0] = PaperVertex(-hw, hh, z , 0.0f, 0.0f);
		out_v[1] = PaperVertex( hw, hh, z , 1.0f, 0.0f);
		out_v[2] = PaperVertex( hw,-hh, z , 1.0f, 1.0f);
		out_v[3] = PaperVertex(-hw,-hh, z , 0.0f, 1.0f);

		out_i.resize(6);
		out_i[0] = 0; 		out_i[1] = 1; 		out_i[2] = 2;
		out_i[3] = 0; 		out_i[4] = 2; 		out_i[5] = 3;
	}
	static void EmptyRectangle(float width, float height, float z, float thick, std::vector<PaperVertex>& out_v, std::vector<uint32_t>& out_i)
	{
		out_v.resize(12);
		out_i.resize(24);

		float hw = width * 0.5f;
		float hh = height * 0.5f;

		float out_hw = hw;
		float out_hh = hh;

		float in_hw = hw - (thick * 2);
		float in_hh = hh - (thick * 2);

		float t_thick = 1.0f / thick;

		out_v[0] = PaperVertex(-out_hw, +in_hh,  z, 0, t_thick);
		out_v[1] = PaperVertex(-out_hw, +out_hh, z, 0, 0);
		out_v[2] = PaperVertex(+out_hw, +out_hh, z, 1, 0);
		out_v[3] = PaperVertex(+out_hw, +in_hh,  z, 1, t_thick);

		out_v[4] = PaperVertex(-out_hw, -out_hh,z, 0, 1);
		out_v[5] = PaperVertex(-out_hw, -in_hh, z, 0, 1 - t_thick);
		out_v[6] = PaperVertex(+out_hw, -in_hh, z, 1, 1 - t_thick);
		out_v[7] = PaperVertex(+out_hw, -out_hh,z, 1, 1);


		out_v[8] = PaperVertex(-in_hw, -in_hh,  z,t_thick, 1 - t_thick);
		out_v[9] = PaperVertex(-in_hw, +in_hh,  z, t_thick, t_thick);
		out_v[10] = PaperVertex(+in_hw, +in_hh, z, 1 - t_thick, t_thick);
		out_v[11] = PaperVertex(+in_hw, -in_hh, z, 1 - t_thick, 1 - t_thick);





		// 위 막대기
		out_i[0] = 0; out_i[1] = 1; out_i[2] = 2;
		out_i[3] = 0; out_i[4] = 2; out_i[5] = 3;

		// 아래 막대기
		out_i[6] = 4; out_i[7] = 5; out_i[8] = 6;
		out_i[9] = 4; out_i[10] = 6; out_i[11] = 7;

		// 왼쪽 막대기
		out_i[12] = 5; out_i[13] = 0; out_i[14] = 9;
		out_i[15] = 5; out_i[16] = 9; out_i[17] = 8;


		// 오른쪽 막대기
		out_i[18] = 11; out_i[19] = 10; out_i[20] = 3;
		out_i[21] = 11; out_i[22] = 3; out_i[23] = 6;

	}
};