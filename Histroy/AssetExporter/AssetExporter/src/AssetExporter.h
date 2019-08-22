#pragma once
#include<iostream>
#include<string>
#define MESH_FILEFORMAT ".mesh"
#define SKEELETAL_FILEFORMAT ".skeletal"
#define ANIM_FILEFORMAT ".anim"

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiBone;
struct aiAnimMesh;
struct aiAnimation;
struct aiVectorKey;
struct aiQuatKey;
struct aiNodeAnim;



namespace AssetExporter
{

	/*    .mesh 데이터 

	1. 메시 데이터 갯수
	1. bool is_skeletal : 스켈레탈 메시(true) 인가 스테틱 메시(false) 인가

	// -> 여기 까지 공통 변수 로 읽기

	// -> 이후 부터는 메시 데이터 갯수 만큼 읽기
	2. uint32_t len   : 메시 이름 문자열 갯수
	3. char name[len] : 메시 이름 
	4. uint32_t vertexCount : Vertex 갯수
	5. uint32_t indexCount :  Index  갯수
	6. 각 스태틱이나 스켈레탈 메시 정점 데이터
	7. 각 스태틱 이나 스켈레탈 메시 인덱스 데이터

	*/

	enum EOption
	{
		opt_Mesh = 0x00001,
		opt_SkeletalMesh = 0x00002,
		opt_MeshCombined = 0x00004,
		opt_Animation = 0x0008
	};

	enum EError
	{
		Failed_Read_File,
		NoneTangent,
		NoneTexcoord
	};

	std::string ErrorToString(EError error);

	void Execute(const std::string& path, const std::string& outputpath, EOption option, std::vector<EError>* outputError = nullptr);;
};