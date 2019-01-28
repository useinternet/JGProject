#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include"ModelLoader.h"
#include"Data/Mesh/JGSkeletalMesh.h"
#include"Data/Animation/JGAnimation.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
XMFLOAT4X4 AiToXm(const aiMatrix4x4& m)
{

	DirectX::XMFLOAT4X4 mat;
	mat._11 = m.a1; mat._12 = m.a2; mat._13 = m.a3; mat._14 = m.a4;
	mat._21 = m.b1; mat._22 = m.b2; mat._23 = m.b3; mat._24 = m.b4;
	mat._31 = m.c1; mat._32 = m.c2; mat._33 = m.c3; mat._34 = m.c4;
	mat._41 = m.d1; mat._42 = m.d2; mat._43 = m.d3; mat._44 = m.d4;
	return mat;
}
XMFLOAT3 AiToXm(const aiVector3D& v)
{
	return XMFLOAT3(v.x, v.y, v.z);
}
XMFLOAT4 AiToXm(const aiQuaternion& q)
{
	return XMFLOAT4(q.x, q.y, q.z, q.w);
}
ModelLoader::ModelLoader(
	const string&                        path, 
	vector<GeometryGenerator::MeshData>* MeshData, 
	vector<string>*                      MeshName)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(path, 
		aiProcess_JoinIdenticalVertices |        // 동일한 꼭지점 결합, 인덱싱 최적화
		aiProcess_ValidateDataStructure |        // 로더의 출력을 검증
		aiProcess_ImproveCacheLocality |        // 출력 정점의 캐쉬위치를 개선
		aiProcess_RemoveRedundantMaterials |    // 중복된 매터리얼 제거
		aiProcess_GenUVCoords |                    // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
		aiProcess_TransformUVCoords |            // UV 변환 처리기 (스케일링, 변환...)
		aiProcess_FindInstances |                // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
		aiProcess_LimitBoneWeights |            // 정점당 뼈의 가중치를 최대 4개로 제한
		aiProcess_OptimizeMeshes |                // 가능한 경우 작은 매쉬를 조인
		aiProcess_GenSmoothNormals |            // 부드러운 노말벡터(법선벡터) 생성
		aiProcess_SplitLargeMeshes |            // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
		aiProcess_Triangulate |                    // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
		aiProcess_ConvertToLeftHanded |            // D3D의 왼손좌표계로 변환
		aiProcess_SortByPType     |  // 단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬
		aiProcess_CalcTangentSpace);                    

	if (pScene == nullptr)
		Success = false;
	else
	{
		ProcessNode(pScene->mRootNode, pScene, MeshData, MeshName);
	}
}
ModelLoader::ModelLoader(
	const string&                                path, 
	vector<GeometryGenerator::SkinnedMeshData>*  MeshData,
	unordered_map<string, vector<JGBoneData>>*   bonedatas, 
	unordered_map<string, JGBoneNode*>*          boneHierarchy,
	vector<string>*                              MeshName)
{


	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(path,
		aiProcess_JoinIdenticalVertices |        // 동일한 꼭지점 결합, 인덱싱 최적화
		aiProcess_ValidateDataStructure |        // 로더의 출력을 검증
		aiProcess_ImproveCacheLocality |        // 출력 정점의 캐쉬위치를 개선
		aiProcess_RemoveRedundantMaterials |    // 중복된 매터리얼 제거
		aiProcess_GenUVCoords |                    // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
		aiProcess_TransformUVCoords |            // UV 변환 처리기 (스케일링, 변환...)
		aiProcess_FindInstances |                // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
		aiProcess_LimitBoneWeights |            // 정점당 뼈의 가중치를 최대 4개로 제한
		aiProcess_OptimizeMeshes |                // 가능한 경우 작은 매쉬를 조인
		aiProcess_GenSmoothNormals |            // 부드러운 노말벡터(법선벡터) 생성
		aiProcess_SplitLargeMeshes |            // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
		aiProcess_Triangulate |                    // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
		aiProcess_ConvertToLeftHanded |            // D3D의 왼손좌표계로 변환
		aiProcess_SortByPType |                 // 단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬
		aiProcess_CalcTangentSpace );                   

	if (pScene == nullptr)
		Success = false;
	else
	{
		ProcessNode(pScene->mRootNode, pScene, MeshData, bonedatas, MeshName);
		for (auto& name : *MeshName)
		{
			m_NodeIndex = 0;
			JGBoneNode* Root = JGBoneNode::CreateNode();
			ProcessBoneHierarchy(pScene->mRootNode, Root, &(*bonedatas)[name]);
			(*boneHierarchy)[name] = Root;
		}
	}

}
ModelLoader::ModelLoader(const string& path, vector<JGAnimation>* AnimArray)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(path,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		Success = false;
	else
	{
		aiMatrix4x4 TempMat = pScene->mRootNode->mTransformation;
		TempMat.Transpose().Inverse();
		for (UINT i = 0; i < pScene->mNumAnimations; ++i)
		{
			JGAnimation Anim;
			

			Anim.InvGlobalTransform = AiToXm(TempMat);



			aiAnimation* CurrAnim = pScene->mAnimations[i];
			Anim.Name          = CurrAnim->mName.C_Str();
			Anim.Duration      = (float)CurrAnim->mDuration;
			Anim.TickPerSecond = (float)CurrAnim->mTicksPerSecond;
			ProcessAnimation(CurrAnim, &Anim);
			AnimArray->push_back(Anim);
		}

	}
}
void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, vector<GeometryGenerator::MeshData>* MeshData, vector<std::string>* MeshName)
{
	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		GeometryGenerator::MeshData data;
		ProcessMesh(mesh, scene, &data);
		MeshData->push_back(data);
		MeshName->push_back(mesh->mName.C_Str());
	}
	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		this->ProcessNode(node->mChildren[i], scene, MeshData, MeshName);
	}
}
void ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, GeometryGenerator::MeshData* meshData)
{

	// 정점
	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		
		GeometryGenerator::Vertex v;
		v.Position.x = mesh->mVertices[i].x;
		v.Position.y = mesh->mVertices[i].y;
		v.Position.z = mesh->mVertices[i].z;
		v.Normal.x = mesh->mNormals[i].x;
		v.Normal.y = mesh->mNormals[i].y;
		v.Normal.z = mesh->mNormals[i].z;
		if (mesh->mTangents != nullptr)
		{
			v.TangentU.x = mesh->mTangents[i].x;
			v.TangentU.y = mesh->mTangents[i].y;
			v.TangentU.z = mesh->mTangents[i].z; 
			IsTangent = false;
		}


		if (mesh->mTextureCoords[0])
		{
			v.TexC.x = (float)mesh->mTextureCoords[0][i].x;
			v.TexC.y = (float)mesh->mTextureCoords[0][i].y;
		}
		meshData->Vertices.push_back(v);
	}

	// 인덱스
	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; ++j)
		{
			meshData->Indices32.push_back(face.mIndices[j]);
		}
	}
}


//


void ModelLoader::ProcessNode(
	aiNode*                                     node, 
	const aiScene*                              scene,
	vector<GeometryGenerator::SkinnedMeshData>* MeshData,
	unordered_map<string, vector<JGBoneData>>*  bonedatas,
	vector<string>*                             MeshName)
{
	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{	
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		GeometryGenerator::SkinnedMeshData data;
		vector<JGBoneData> bones;
		MeshName->push_back(mesh->mName.C_Str());

		ProcessMesh(mesh, scene, &data, &bones);
		(*bonedatas)[mesh->mName.C_Str()] = bones;
		MeshData->push_back(data);
	}
	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		this->ProcessNode(node->mChildren[i], scene, MeshData, bonedatas, MeshName);
	}
}
void ModelLoader::ProcessMesh(
	aiMesh*                             mesh,
	const aiScene*                      scene, 
	GeometryGenerator::SkinnedMeshData* meshData,
	vector<JGBoneData>*                 boneData)
{
	// 정점
	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		GeometryGenerator::SkinnedVertex v;
		v.Position.x = mesh->mVertices[i].x;
		v.Position.y = mesh->mVertices[i].y;
		v.Position.z = mesh->mVertices[i].z;
		v.Normal.x = mesh->mNormals[i].x;
		v.Normal.y = mesh->mNormals[i].y;
		v.Normal.z = mesh->mNormals[i].z;
		if (mesh->mTangents != nullptr)
		{
			v.TangentU.x = mesh->mTangents[i].x;
			v.TangentU.y = mesh->mTangents[i].y;
			v.TangentU.z = mesh->mTangents[i].z;
			IsTangent = true;
		}


		if (mesh->mTextureCoords[0])
		{
			v.TexC.x = (float)mesh->mTextureCoords[0][i].x;
			v.TexC.y = (float)mesh->mTextureCoords[0][i].y;
		}
		meshData->Vertices.push_back(v);
	}
	unordered_map<string, UINT> BoneMapping;
	UINT BoneOffset = 0;
	UINT BoneIndex = BoneOffset;
	for (UINT i = 0; i < mesh->mNumBones; ++i)
	{
		JGBoneData data;
		aiBone* CurrBone = mesh->mBones[i];
		string  BoneName = (CurrBone->mName.C_Str());
		if (BoneMapping.find(BoneName) == BoneMapping.end())
		{
			BoneIndex = BoneOffset;
			BoneOffset++;
			BoneMapping[BoneName] = BoneIndex;


			aiMatrix4x4 TempMat = CurrBone->mOffsetMatrix;
			data.Name   = BoneName;
			data.Offset = AiToXm(TempMat.Transpose());
			data.Index  = BoneIndex;
			boneData->push_back(data);
		}
		else
		{
			BoneIndex = BoneMapping[BoneName];
		}
		for (UINT j = 0; j < CurrBone->mNumWeights; ++j)
		{
			UINT vID = CurrBone->mWeights[j].mVertexId;
			float w = CurrBone->mWeights[j].mWeight;
			AddBoneData(BoneIndex, w, vID, meshData);
		}
	}
	// 인덱스
	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; ++j)
		{
			meshData->Indices32.push_back(face.mIndices[j]);
		}
	}
}
void ModelLoader::ProcessBoneHierarchy(
	aiNode*             node,
	JGBoneNode*         boneNode,
    vector<JGBoneData>* bonedatas)
{
	JGBoneNode* parent = boneNode;
	if (node->mParent == nullptr)
	{
		boneNode->Data.Name  = "RootNode";
		boneNode->Data.Index = -1;
	}
	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		aiNode* ChildNode = node->mChildren[i];
		string  ChildName = ChildNode->mName.C_Str();
		if (ChildName.length() != 0 && ChildNode->mNumMeshes == 0)
		{
			boneNode = JGBoneNode::CreateNode();
			boneNode->Parent = parent;
			boneNode->NodeIndex = m_NodeIndex++;
			//
			aiMatrix4x4 NodeTransform = node->mTransformation;
			boneNode->Transform = AiToXm(NodeTransform.Transpose());

			for (auto& bone : *bonedatas)
			{
				if (bone.Name == ChildName)
				{
					boneNode->bExist = true;
					boneNode->Data = bone;
					break;
				}
			}
			boneNode->Data.Name = ChildName;
			parent->Child.push_back(boneNode);
	
		}
		ProcessBoneHierarchy(node->mChildren[i], boneNode, bonedatas);
	}
}
void ModelLoader::ProcessAnimation(aiAnimation* Anim, class JGAnimation* OutputAnim)
{
	for (UINT i = 0; i < Anim->mNumChannels; ++i)
	{
		JGAnimChannel channel;
		aiNodeAnim* CurrNode = Anim->mChannels[i];
		channel.Name = CurrNode->mNodeName.C_Str();
		
		for (UINT j = 0; j < CurrNode->mNumPositionKeys; ++j)
		{
			JGVectorFrame frame;
			frame.TimePos = (float)CurrNode->mPositionKeys[j].mTime;
			frame.Value   = AiToXm(CurrNode->mPositionKeys[j].mValue);
			channel.AnimPositionFrames.push_back(frame);
		}
		for (UINT j = 0; j < CurrNode->mNumRotationKeys; ++j)
		{
			JGQuatFrame frame;
			frame.TimePos = (float)CurrNode->mRotationKeys[j].mTime;
			frame.Value   = AiToXm(CurrNode->mRotationKeys[j].mValue);
			channel.AnimRotationFrames.push_back(frame);
		}
		for (UINT j = 0; j < CurrNode->mNumScalingKeys; ++j)
		{
			JGVectorFrame frame;
			frame.TimePos = (float)CurrNode->mScalingKeys[j].mTime;
			frame.Value   = AiToXm(CurrNode->mScalingKeys[j].mValue);
			channel.AnimScaleFrames.push_back(frame);
		}
		OutputAnim->AnimChannels.push_back(channel);

	}
}
void ModelLoader::AddBoneData(UINT id, float w, UINT index, GeometryGenerator::SkinnedMeshData* meshData)
{
	if (meshData->Vertices[index].BoneWeights.x == 0)
	{
		meshData->Vertices[index].BoneIndices[0] = id;
		meshData->Vertices[index].BoneWeights.x = w;
		return;
	}
	if (meshData->Vertices[index].BoneWeights.y == 0)
	{
		meshData->Vertices[index].BoneIndices[1] = id;
		meshData->Vertices[index].BoneWeights.y = w;
		return;
	}
	if (meshData->Vertices[index].BoneWeights.z == 0)
	{
		meshData->Vertices[index].BoneIndices[2] = id;
		meshData->Vertices[index].BoneWeights.z = w;
		return;
	}
	if (meshData->Vertices[index].BoneWeights.w == 0)
	{
		meshData->Vertices[index].BoneIndices[3] = id;
		meshData->Vertices[index].BoneWeights.w = w;
		return;
	}
}