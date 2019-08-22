#include"pch.h"
#include<filesystem>
#include"AssetExporter.h"
#include"assimp/Importer.hpp"
#include"assimp/scene.h"
#include"assimp/postprocess.h"
#pragma comment(lib, "assimp.lib")

using namespace std;
using namespace Assimp;
namespace fs = experimental::filesystem;
namespace AssetExporter
{
	// 데이터 타입 정의 
	struct Vector3
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	struct Vector2
	{
		float x = 0.0f;
		float y = 0.0f;
	};
	struct Matrix4x4
	{
		float m[4][4];
	};
	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
		Vector3 tangent;
		Vector2 tex;
	};
	using Index = uint32_t;
	struct MeshData
	{
		std::string Name;
		std::vector<Vertex>   Vertices;
		std::vector<Index>    Indices;
	};


	void PushError(EError error, vector<EError>* outputError)
	{
		if (outputError)
		{
			outputError->push_back(error);
		}
	}

	// 데이터 로드
	void LoadProcessMesh(aiMesh* aimesh, MeshData& output, vector<EError>* outputError, uint32_t IndexOffset);
	void LoadMesh(aiNode* node, const aiScene* pScene, std::vector<MeshData>& output,vector<EError>* outputError);
	// 데이터 쓰기
	void WriteMeshFile(std::vector<MeshData>& output, const std::string& outputpath);
	void WriteMeshFile(std::vector<MeshData>& output, const std::string& outputpath, const std::string& filename);
	std::string ErrorToString(EError error)
	{
		switch (error)
		{
		case Failed_Read_File:
			return "Failed Read File..";
		case NoneTangent:
			return "None Tangent";
		case NoneTexcoord:
			return "None Texcoord";
		}
		return "None";
	}
	void Execute(const std::string& path, const std::string& outputpath, EOption option, vector<EError>* outputError)
	{
		Importer importer;

		const aiScene* pScene = importer.ReadFile(path,
			aiProcess_JoinIdenticalVertices |  // 동일한 꼭지점 결합, 인덱싱 최적화
			aiProcess_ValidateDataStructure |  // 로더의 출력을 검증
			aiProcess_ImproveCacheLocality |  // 출력 정점의 캐쉬위치를 개선
			aiProcess_RemoveRedundantMaterials |  // 중복된 매터리얼 제거
			aiProcess_GenUVCoords |  // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
			aiProcess_TransformUVCoords |  // UV 변환 처리기 (스케일링, 변환...)
			aiProcess_FindInstances |  // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
			aiProcess_LimitBoneWeights |  // 정점당 뼈의 가중치를 최대 4개로 제한
			aiProcess_OptimizeMeshes |  // 가능한 경우 작은 매쉬를 조인
			aiProcess_GenSmoothNormals |  // 부드러운 노말벡터(법선벡터) 생성
			aiProcess_SplitLargeMeshes |  // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
			aiProcess_Triangulate |  // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
			aiProcess_ConvertToLeftHanded |  // D3D의 왼손좌표계로 변환
			aiProcess_SortByPType |  // 단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬
			aiProcess_CalcTangentSpace); // 탄젠트 공간 계산 

		if (pScene == nullptr)
		{
			PushError(Failed_Read_File, outputError);
			return;
		}
		else
		{


			if (option & opt_Mesh)
			{
				vector<MeshData> output;
				LoadMesh(pScene->mRootNode, pScene, output, outputError);
				if (option & opt_MeshCombined)
				{
					fs::path fpath(path);
					std::string filename = fpath.filename().string();
					filename = filename.substr(0, filename.length() - 4);
					WriteMeshFile(output, outputpath, filename);
				}
				else
				{
					WriteMeshFile(output, outputpath);
				}
			
			}
		}


	}

	void LoadMesh(aiNode* node, const aiScene* pScene, std::vector<MeshData>& outputData, vector<EError>* outputError)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* aimesh = pScene->mMeshes[node->mMeshes[i]];
			MeshData data;
			LoadProcessMesh(aimesh, data, outputError, 0);
			outputData.push_back(data);
		}

		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			LoadMesh(node->mChildren[i], pScene, outputData, outputError);
		}

	}
	void LoadProcessMesh(aiMesh* aimesh, MeshData& output, vector<EError>* outputError, uint32_t IndexOffset)
	{
		// Vertex Data 저장
		for (uint32_t i = 0; i < aimesh->mNumVertices; ++i)
		{
			Vertex v;

			v.position.x = aimesh->mVertices[i].x;
			v.position.y = aimesh->mVertices[i].y;
			v.position.z = aimesh->mVertices[i].z;

			v.normal.x = aimesh->mNormals[i].x;
			v.normal.y = aimesh->mNormals[i].y;
			v.normal.z = aimesh->mNormals[i].z;

			if (aimesh->mTangents != nullptr)
			{
				v.tangent.x = aimesh->mTangents[i].x;
				v.tangent.y = aimesh->mTangents[i].y;
				v.tangent.z = aimesh->mTangents[i].z;
			}
			else
			{
				PushError(NoneTangent, outputError);
			}



			if (aimesh->mTextureCoords[0])
			{
				v.tex.x = (float)aimesh->mTextureCoords[0][i].x;
				v.tex.y = (float)aimesh->mTextureCoords[0][i].y;
			}
			else
			{
				PushError(NoneTexcoord, outputError);
			}

			output.Vertices.push_back(v);
		}



		// Index Data 저장
		for (uint32_t i = 0; i < aimesh->mNumFaces; ++i)
		{
			aiFace face = aimesh->mFaces[i];

			for (uint32_t j = 0; j < face.mNumIndices; ++j)
			{
				output.Indices.push_back(IndexOffset + face.mIndices[j]);
			}
		}

		//
		output.Name = aimesh->mName.C_Str();
	}
	void WriteMeshFile(std::vector<MeshData>& output,const std::string& outputpath)
	{
		uint32_t numOutput = (uint32_t)output.size();


		for (uint32_t i = 0; i < numOutput; ++i)
		{
			MeshData& data = output[i];
			auto& vertices = data.Vertices;
			auto& indices = data.Indices;
			string& name = data.Name;
			//Vector3 maxV = { FLT_MIN , FLT_MIN };
			//Vector3 minV = { FLT_MAX, FLT_MAX };
			//for (int j = 0; j < vertices.size(); ++j)
			//{

			//}
			ofstream fout;
			fout.open(outputpath + name + MESH_FILEFORMAT, ios::binary);

			// 메시 데이터 갯수
			uint32_t meshCount = 1;

			fout.write(
				reinterpret_cast<const char*>(&meshCount),
				sizeof(uint32_t)
			);


			bool is_skeletal = false;

			// 스케레탈 메시 여부
			fout.write(
				reinterpret_cast<const char*>(&is_skeletal),
				sizeof(bool)
			);
			// 메시 이름 길이
			uint32_t len = (uint32_t)name.length() + 1;
			fout.write(
				reinterpret_cast<const char*>(&len),
				sizeof(uint32_t)
			);

			// 메시 이름
			const char* c_name = name.c_str();

			fout.write(
				reinterpret_cast<const char*>(c_name),
				sizeof(char) * len
			);

			// Vertex 갯 수
			uint32_t vertexCount = (uint32_t)vertices.size();
			fout.write(
				reinterpret_cast<const char*>(&vertexCount),
				sizeof(uint32_t)
			);

			// Index 갯 수

			uint32_t indexCount = (uint32_t)indices.size();
			fout.write(
				reinterpret_cast<const char*>(&indexCount),
				sizeof(uint32_t)
			);

			// Vertex 

			fout.write(
				reinterpret_cast<const char*>(vertices.data()),
				sizeof(Vertex) * vertexCount
			);

			// Index
			fout.write(
				reinterpret_cast<const char*>(indices.data()),
				sizeof(Index) * indexCount
			);


			fout.close();
		}


	}
	void WriteMeshFile(std::vector<MeshData>& output, const std::string& outputpath, const std::string& filename)
	{
		uint32_t numOutput = (uint32_t)output.size();
		ofstream fout;
		fout.open(outputpath + filename + MESH_FILEFORMAT, ios::binary);

		// 메시 갯수
		fout.write(
			reinterpret_cast<const char*>(&numOutput),
			sizeof(uint32_t)
		);
		// 스케레탈 메시 여부
		bool is_skeletal = false;
		fout.write(
			reinterpret_cast<const char*>(&is_skeletal),
			sizeof(bool)
		);

		for (uint32_t i = 0; i < numOutput; ++i)
		{
			MeshData& data = output[i];
			auto& vertices = data.Vertices;
			auto& indices = data.Indices;
			string& name = data.Name;
			



			// 메시 이름 길이
			uint32_t len = (uint32_t)name.length() + 1;
			fout.write(
				reinterpret_cast<const char*>(&len),
				sizeof(uint32_t)
			);

			// 메시 이름
			const char* c_name = name.c_str();

			fout.write(
				reinterpret_cast<const char*>(c_name),
				sizeof(char) * len
			);

			// Vertex 갯 수
			uint32_t vertexCount = (uint32_t)vertices.size();
			fout.write(
				reinterpret_cast<const char*>(&vertexCount),
				sizeof(uint32_t)
			);

			// Index 갯 수

			uint32_t indexCount = (uint32_t)indices.size();
			fout.write(
				reinterpret_cast<const char*>(&indexCount),
				sizeof(uint32_t)
			);

			// Vertex 

			fout.write(
				reinterpret_cast<const char*>(vertices.data()),
				sizeof(Vertex) * vertexCount
			);

			// Index
			fout.write(
				reinterpret_cast<const char*>(indices.data()),
				sizeof(Index) * indexCount
			);



		}





		fout.close();


	}
}



