#include"ResourceReader.h"
#include"Data/Animation/JGAnimation.h"
#include"Data/Mesh/JGSkeletalMesh.h"

using namespace JGRC;
using namespace std;


ResourceReader::ResourceReader(
	const std::string& path,
	GeometryGenerator::MeshData& MeshData) 
{



}
ResourceReader::ResourceReader(
	const std::string& path,
	GeometryGenerator::SkinnedMeshData& MeshData,
	vector<JGBoneData>& BoneData,
	JGBoneNode** Root)
{
	*Root = JGBoneNode::CreateNode();
	(*Root)->bExist = false;
	(*Root)->NodeIndex = -1;
	(*Root)->Data.Name = "RootNode";
	(*Root)->Parent = nullptr;
	(*Root)->Transform = MathHelper::Identity4x4();
	ReadSkeletalMesh(path, MeshData, BoneData, *Root);
}
ResourceReader::ResourceReader(
	const string& path,
	JGAnimation& Anim)
{
	ReadAnimation(path, Anim);
}

void ResourceReader::ReadSkeletalMesh(const string& path, GeometryGenerator::SkinnedMeshData& MeshData, vector<JGBoneData>& BoneData, JGBoneNode* Root)
{
	ifstream fin;
	fin.open(path);
	if (!fin.is_open())
	{
		Success = false;
		return;
	}
	else
		Success = true;

	string buffer;
	fin >> buffer >> buffer >> buffer;
	// 정점 갯수
	UINT VertexCount = 0;
	fin >> VertexCount;

	// 인덱스 갯수
	UINT IndexCount = 0;
	fin >> buffer >> buffer;
	fin >> IndexCount;

	// 본 갯수
	UINT BoneCount = 0;
	fin >> buffer >> buffer;
	fin >> BoneCount;

	// 데이터 추출
	for (UINT i = 0; i < VertexCount; ++i)
	{
		GeometryGenerator::SkinnedVertex v;

		fin >> v.Position.x >> v.Position.y >> v.Position.z;
		fin >> v.Normal.x   >> v.Normal.y   >> v.Normal.z;
		fin >> v.TangentU.x >> v.TangentU.y >> v.TangentU.z;
		fin >> v.TexC.x     >> v.TexC.y;
		fin >> v.BoneIndices[0] >> v.BoneIndices[1] >> v.BoneIndices[2] >> v.BoneIndices[3];
		fin >> v.BoneWeights.x  >> v.BoneWeights.y  >> v.BoneWeights.z  >> v.BoneWeights.w;
		MeshData.Vertices.push_back(v);
	}
	for (UINT i = 0; i < IndexCount; ++i)
	{
		std::uint32_t Index;
		fin >> Index;
		MeshData.Indices32.push_back(Index);
	}
	for (UINT i = 0; i < BoneCount; ++i)
	{
		JGBoneData bone;
		fin >> bone.Name;
		fin >> bone.Index;
		fin >> bone.Offset._11 >> bone.Offset._12 >> bone.Offset._13 >> bone.Offset._14;
		fin >> bone.Offset._21 >> bone.Offset._22 >> bone.Offset._23 >> bone.Offset._24;
		fin >> bone.Offset._31 >> bone.Offset._32 >> bone.Offset._33 >> bone.Offset._34;
		fin >> bone.Offset._41 >> bone.Offset._42 >> bone.Offset._43 >> bone.Offset._44;
		BoneData.push_back(bone);
	}
	unordered_map<int, JGBoneNode*> BoneNodes;
	unordered_map<int, pair<int,vector<int>>> BoneHierarchy;
	while (!fin.eof())
	{
		JGBoneNode* node = JGBoneNode::CreateNode();
		JGBoneData data;
		fin >> node->bExist;
		fin >> data.Name;
		fin >> data.Index;
		fin >> data.Offset._11 >> data.Offset._12 >> data.Offset._13 >> data.Offset._14;
		fin >> data.Offset._21 >> data.Offset._22 >> data.Offset._23 >> data.Offset._24;
		fin >> data.Offset._31 >> data.Offset._32 >> data.Offset._33 >> data.Offset._34;
		fin >> data.Offset._41 >> data.Offset._42 >> data.Offset._43 >> data.Offset._44;
		node->Data = data;
		
		fin >> node->NodeIndex;
		int parentIndex  = 0;
		int childCount   = 0;
	

		fin >> parentIndex;
		fin >> childCount;
		vector<int> childIndexs(childCount);
		for (int i = 0; i < childCount; ++i)
		{
			fin >> childIndexs[i];
		}


		fin >> node->Transform._11 >> node->Transform._12 >> node->Transform._13 >> node->Transform._14;
		fin >> node->Transform._21 >> node->Transform._22 >> node->Transform._23 >> node->Transform._24;
		fin >> node->Transform._31 >> node->Transform._32 >> node->Transform._33 >> node->Transform._34;
		fin >> node->Transform._41 >> node->Transform._42 >> node->Transform._43 >> node->Transform._44;

		BoneNodes[node->NodeIndex] = node;
		BoneHierarchy[node->NodeIndex].first = parentIndex;
		BoneHierarchy[node->NodeIndex].second = move(childIndexs);
	}
	fin.close();


	for(auto& node : BoneNodes)
	{
		if (node.first == -1)
			continue;
		int parentIndex = BoneHierarchy[node.second->NodeIndex].first;

		if (parentIndex == -1)
		{
			node.second->Parent = Root;
			Root->Child.push_back(node.second);
		}
		else
		{
			node.second->Parent = BoneNodes[parentIndex];
		}
			
			

		for (UINT j = 0; j < BoneHierarchy[node.second->NodeIndex].second.size(); ++j)
		{
			int childIndex = BoneHierarchy[node.second->NodeIndex].second[j];
			node.second->Child.push_back(BoneNodes[childIndex]);
		}
	}
}
void ResourceReader::ReadAnimation(const string& path, JGAnimation& Anim)
{
	ifstream fin;
	fin.open(path);
	if (!fin.is_open())
	{
		Success = false;
		return;
	}
	else
		Success = true;
	string buffer;
	fin >> Anim.Name;
	fin >> buffer >> buffer >> Anim.Duration;
	fin >> buffer >> buffer >> Anim.TickPerSecond;
	fin >> buffer;
	fin >> Anim.InvGlobalTransform._11 >> Anim.InvGlobalTransform._12 >> Anim.InvGlobalTransform._13 >> Anim.InvGlobalTransform._14;
	fin >> Anim.InvGlobalTransform._21 >> Anim.InvGlobalTransform._22 >> Anim.InvGlobalTransform._23 >> Anim.InvGlobalTransform._24;
	fin >> Anim.InvGlobalTransform._31 >> Anim.InvGlobalTransform._32 >> Anim.InvGlobalTransform._33 >> Anim.InvGlobalTransform._34;
	fin >> Anim.InvGlobalTransform._41 >> Anim.InvGlobalTransform._42 >> Anim.InvGlobalTransform._43 >> Anim.InvGlobalTransform._44;

	UINT ChannelCount = 0;
	fin >> buffer >> buffer >> ChannelCount;
	Anim.AnimChannels.resize(ChannelCount);

	// 데이터 로드
	for (UINT i = 0; i < ChannelCount; ++i)
	{
		JGAnimChannel ch;
		fin >> ch.Name;

		// Position Frame..
		UINT PosFrameCount   = 0;
		fin >> buffer >> buffer >> PosFrameCount;
		ch.AnimPositionFrames.resize(PosFrameCount);
		for (UINT j = 0; j < PosFrameCount; ++j)
		{
			JGVectorFrame frame;
			fin >> frame.TimePos;
			fin >> frame.Value.x >> frame.Value.y >> frame.Value.z;
			ch.AnimPositionFrames[j] = frame;
		}
		// Scale Frame..
		UINT ScaleFrameCount = 0;
		fin >> buffer >> buffer >> ScaleFrameCount;
		ch.AnimScaleFrames.resize(ScaleFrameCount);
		for (UINT j = 0; j < ScaleFrameCount; ++j)
		{
			JGVectorFrame frame;
			fin >> frame.TimePos;
			fin >> frame.Value.x >> frame.Value.y >> frame.Value.z;
			ch.AnimScaleFrames[j] = frame;
		}
		// Quat Frame..

		UINT QuatFrameCount = 0;
		fin >> buffer >> buffer >> QuatFrameCount;
		ch.AnimRotationFrames.resize(QuatFrameCount);
		for (UINT j = 0; j < QuatFrameCount; ++j)
		{
			JGQuatFrame frame;
			fin >> frame.TimePos;
			fin >> frame.Value.x >> frame.Value.y >> frame.Value.z >> frame.Value.w;
			ch.AnimRotationFrames[j] = frame;
		}

		Anim.AnimChannels[i] = ch;
	}
}