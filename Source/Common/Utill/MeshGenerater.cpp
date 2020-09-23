#pragma once
#include "pch.h"
#include "MeshGenerater.h"
#include "App/GlobalSharedData.h"
#include "App/AssetManager.h"
//#include "ThirdParty/assimp/"
using namespace std;

bool MeshGenerator::LoadFromFile(const std::wstring& path, std::vector<StaticMeshData>& out_meshDatas)
{
	std::ifstream fin;
	fin.open(path, ios::binary);
	if (!fin.is_open())
	{
		return false;
	}
	// numOutput °¹¼ö
	uint32_t numOutput = 0;
	fin.read(
		reinterpret_cast<char*>(&numOutput),
		sizeof(uint32_t));
	out_meshDatas.resize(numOutput);


	// ½ºÄÉ·¹Å» ¿©ºÎ
	bool is_skeletal = false;
	fin.read(
		reinterpret_cast<char*>(&is_skeletal),
		sizeof(bool));
	if (is_skeletal) return false;

	for (uint32_t i = 0; i < numOutput; ++i)
	{
		// Vertex °¹ ¼ö
		uint32_t vertexCount = 0;
		fin.read(
			reinterpret_cast<char*>(&vertexCount),
			sizeof(uint32_t)
		);

		// Index °¹ ¼ö

		uint32_t indexCount = 0;
		fin.read(
			reinterpret_cast<char*>(&indexCount),
			sizeof(uint32_t)
		);

		// Vertex 
		out_meshDatas[i].v.resize(vertexCount);
		fin.read(
			reinterpret_cast<char*>(out_meshDatas[i].v.data()),
			sizeof(StaticVertex) * vertexCount
		);

		// Index
		out_meshDatas[i].i.resize(indexCount);
		fin.read(
			reinterpret_cast<char*>(out_meshDatas[i].i.data()),
			sizeof(uint32_t) * indexCount
		);

		fin.read(
			reinterpret_cast<char*>(&out_meshDatas[i].bbox),
			sizeof(JBBox)
		);
	}


	
	fin.close();
	return true;
}

