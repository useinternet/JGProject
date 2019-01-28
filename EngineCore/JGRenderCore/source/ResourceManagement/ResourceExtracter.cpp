#include"ResourceExtracter.h"
#include"Data/Animation/JGAnimation.h"
#include"Data/Mesh/JGSkeletalMesh.h"
#include"DxCommon/ModelLoader.h"
using namespace JGRC;
using namespace std;

ResourceExtracter::ResourceExtracter(EExtracter_ResourceType type, const string& outputPath, const string& path)
{
	m_OutputPath = outputPath;
	if (type & Resource_Extracter_StaticMesh)
	{
		ModelLoader loader(path, &m_MeshData, &m_MeshName);
		StaticMeshSuccess = loader.Success;
		if (StaticMeshSuccess)
			WriteStaticMeshData();
	}
	if (type & Resource_Extracter_SkeletalMesh)
	{
		ModelLoader loader(path, &m_SkinnedMeshData, &m_BoneData, &m_BoneHierarchy, &m_SkinnedMeshName);
		SkeletalMeshSuccess = loader.Success;
		if (SkeletalMeshSuccess)
			WriteSkeletalMeshData();
	}
	if (type & Resource_Extracter_Animation)
	{
		ModelLoader loader(path, &m_Animation);
		AnimationSuccess = loader.Success;
		if (AnimationSuccess)
			WriteAnimation(); 
	}
}
void ResourceExtracter::WriteStaticMeshData()
{

	string path = m_OutputPath;
	char end = *(path.end() - 1);
	if (end != '/' && end != '\\')
	{
		path += '/';
	}
	UINT len = (UINT)m_MeshData.size();
	for (UINT i = 0; i < len; ++i)
	{
		ofstream fout;
		string filepath = path + m_MeshName[i] + global_static_mesh_file_format;
		fout.open(filepath);
		if (!fout.is_open())
			continue;

		fout << m_MeshName[i] << endl;
		fout << "VertexCount : " << m_MeshData[i].Vertices.size() << endl;
		fout << "IndexCount  : " << m_MeshData[i].Indices32.size() << endl;


		for (auto& vertex : m_MeshData[i].Vertices)
		{
			fout << vertex.Position.x << " " << vertex.Position.y << " " << vertex.Position.z << endl;
			fout << vertex.Normal.x   << " " << vertex.Normal.y   << " " << vertex.Normal.z   << endl;
			fout << vertex.TangentU.x << " " << vertex.TangentU.y << " " << vertex.TangentU.z << endl;
			fout << vertex.TexC.x     << " " << vertex.TexC.y     << endl;
		}
		for (auto& index : m_MeshData[i].Indices32)
		{
			fout << index << " ";
		}
		fout << endl;
		fout.close();
	}
}
void ResourceExtracter::WriteSkeletalMeshData()
{
	string path = m_OutputPath;
	char end = *(path.end() - 1);
	if (end != '/' && end != '\\')
	{
		path += '/';
	}
	UINT len = (UINT)m_SkinnedMeshData.size();

	for (UINT i = 0; i < len; ++i)
	{
		ofstream fout;
		string filepath = path + m_SkinnedMeshName[i] + global_static_skeletal_mesh_file_format;
		fout.open(filepath);

		if (!fout.is_open())
			continue;
		
		fout << m_SkinnedMeshName[i] << endl;
		fout << "VertexCount : " << m_SkinnedMeshData[i].Vertices.size() << endl;
		fout << "IndexCount  : " << m_SkinnedMeshData[i].Indices32.size() << endl;
		fout << "BoneDataCount : " << m_BoneData[m_SkinnedMeshName[i]].size() << endl;



		for (auto& vertex : m_SkinnedMeshData[i].Vertices)
		{
			fout << vertex.Position.x << " " << vertex.Position.y << " " << vertex.Position.z << endl;
			fout << vertex.Normal.x << " " << vertex.Normal.y << " " << vertex.Normal.z << endl;
			fout << vertex.TangentU.x << " " << vertex.TangentU.y << " " << vertex.TangentU.z << endl;
			fout << vertex.TexC.x << " " << vertex.TexC.y << endl;
			fout << vertex.BoneIndices[0] << " " << vertex.BoneIndices[1] << " " <<
				    vertex.BoneIndices[2] << " " << vertex.BoneIndices[3] << endl;
			fout << vertex.BoneWeights.x << " " << vertex.BoneWeights.y << " " << 
				    vertex.BoneWeights.z << " " << vertex.BoneWeights.w << endl;
		}
		for (auto& index : m_SkinnedMeshData[i].Indices32)
		{
			fout << index << " ";
		}
		fout << endl;

		for (auto& bone : m_BoneData[m_SkinnedMeshName[i]])
		{
			fout << bone.Name  << endl;
			fout << bone.Index << endl;
			fout << bone.Offset._11 << " " << bone.Offset._12 << " " << 
				    bone.Offset._13 << " " << bone.Offset._14 << endl;

			fout << bone.Offset._21 << " " << bone.Offset._22 << " " << 
				    bone.Offset._23 << " " << bone.Offset._24 << endl;

			fout << bone.Offset._31 << " " << bone.Offset._32 << " " << 
				    bone.Offset._33 << " " << bone.Offset._34 << endl;

			fout << bone.Offset._41 << " " << bone.Offset._42 << " " << 
				    bone.Offset._43 << " " << bone.Offset._44 << endl;
		}
		WriteBoneHierarchy(fout, m_BoneHierarchy[m_SkinnedMeshName[i]]);
		fout.close();
	}
}
void ResourceExtracter::WriteAnimation()
{
	string path = m_OutputPath;
	char end = *(path.end() - 1);
	if (end != '/' && end != '\\')
	{
		path += '/';
	}
	UINT len = (UINT)m_Animation.size();
	for (UINT i = 0; i < len; ++i)
	{
		ofstream fout;
		string filepath = m_OutputPath + m_Animation[i].Name + global_static_animation_file_format;
		fout.open(filepath);
		if (!fout.is_open())
			continue;
		fout << m_Animation[i].Name << endl;
		fout << "Duration : " << m_Animation[i].Duration << endl;
		fout << "TickPerSecond : " << m_Animation[i].TickPerSecond << endl;
		fout << "InvGlobalTransform" << endl;
		fout << m_Animation[i].InvGlobalTransform._11 << " " << 
			    m_Animation[i].InvGlobalTransform._12 << " " << 
			    m_Animation[i].InvGlobalTransform._13 << " " << 
			    m_Animation[i].InvGlobalTransform._14 << endl;

		fout << m_Animation[i].InvGlobalTransform._21 << " " << 
			    m_Animation[i].InvGlobalTransform._22 << " " << 
			    m_Animation[i].InvGlobalTransform._23 << " " <<
			    m_Animation[i].InvGlobalTransform._24 << endl;

		fout << m_Animation[i].InvGlobalTransform._31 << " " << 
			    m_Animation[i].InvGlobalTransform._32 << " " << 
			    m_Animation[i].InvGlobalTransform._33 << " " << 
			    m_Animation[i].InvGlobalTransform._34 << endl;

		fout << m_Animation[i].InvGlobalTransform._41 << " " << 
			    m_Animation[i].InvGlobalTransform._42 << " " << 
			    m_Animation[i].InvGlobalTransform._43 << " " << 
			    m_Animation[i].InvGlobalTransform._44 << endl;


		fout << "AnimChannelCount : " << m_Animation[i].AnimChannels.size() << endl;
		for (UINT k = 0; k < m_Animation[i].AnimChannels.size(); ++k)
		{
			JGAnimChannel ch = m_Animation[i].AnimChannels[k];

			fout << ch.Name << endl;
			fout << "PosFrameCount : " << ch.AnimPositionFrames.size() << endl;
			for (UINT j = 0; j < ch.AnimPositionFrames.size(); ++j)
			{
				fout << ch.AnimPositionFrames[j].TimePos << endl;
				fout << ch.AnimPositionFrames[j].Value.x << " " <<
					    ch.AnimPositionFrames[j].Value.y << " " <<
					    ch.AnimPositionFrames[j].Value.z << endl;
			}
			fout << "ScaleFrameCount : " << ch.AnimScaleFrames.size() << endl;
			for (UINT j = 0; j < ch.AnimScaleFrames.size(); ++j)
			{
				fout << ch.AnimScaleFrames[j].TimePos << endl;
				fout << ch.AnimScaleFrames[j].Value.x << " " <<
					    ch.AnimScaleFrames[j].Value.y << " " <<
					    ch.AnimScaleFrames[j].Value.z << endl;
			}
			fout << "QuatFrameCount : " << ch.AnimRotationFrames.size() << endl;
			for (UINT j = 0; j < ch.AnimRotationFrames.size(); ++j)
			{
				fout << ch.AnimRotationFrames[j].TimePos << endl;
				fout << ch.AnimRotationFrames[j].Value.x << " " <<
				     	ch.AnimRotationFrames[j].Value.y << " " <<
					    ch.AnimRotationFrames[j].Value.z << " " <<
				    	ch.AnimRotationFrames[j].Value.w << endl;
			}
		}
	}
}
void ResourceExtracter::WriteBoneHierarchy(ofstream& fout, JGBoneNode* Node)
{
	// 일단 존재하는지 
	fout << Node->bExist << endl;	
	fout << Node->Data.Name << endl;
	fout << Node->Data.Index << endl;
	fout << Node->Data.Offset._11 << " " << Node->Data.Offset._12 << " " << 
		    Node->Data.Offset._13 << " " << Node->Data.Offset._14 << endl;
	fout << Node->Data.Offset._21 << " " << Node->Data.Offset._22 << " " << 
		    Node->Data.Offset._23 << " " << Node->Data.Offset._24 << endl;
	fout << Node->Data.Offset._31 << " " << Node->Data.Offset._32 << " " << 
		    Node->Data.Offset._33 << " " << Node->Data.Offset._34 << endl;
	fout << Node->Data.Offset._41 << " " << Node->Data.Offset._42 << " " << 
		    Node->Data.Offset._43 << " " << Node->Data.Offset._44 << endl;
	// 
	fout << Node->NodeIndex << endl;
	(Node->Parent == nullptr) ?
		fout << -1 << endl : 
		fout << Node->Parent->NodeIndex << endl;

	fout << Node->Child.size() << endl;
	for (UINT i = 0; i < Node->Child.size(); ++i)
	{
		fout << Node->Child[i]->NodeIndex << " ";
	}
	if(Node->Child.size() != 0)
		fout << endl;



	// 트랜스 폼
	fout << Node->Transform._11 << " " << Node->Transform._12 << " " << 
		    Node->Transform._13 << " " << Node->Transform._14 << endl;

	fout << Node->Transform._21 << " " << Node->Transform._22 << " " <<
		    Node->Transform._23 << " " << Node->Transform._24 << endl;

	fout << Node->Transform._31 << " " << Node->Transform._32 << " " <<
		    Node->Transform._33 << " " << Node->Transform._34 << endl;

	fout << Node->Transform._41 << " " << Node->Transform._42 << " " <<
		    Node->Transform._43 << " " << Node->Transform._44 << endl;

	for (UINT i = 0; i < Node->Child.size(); ++i)
	{
		WriteBoneHierarchy(fout, Node->Child[i]);
	}
}