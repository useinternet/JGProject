#include<PCH.h>
#include"AssetManager.h"
#include"AssetAnimTransform.h"
#include<unordered_map>
#include<Data/Data_IO.h>
#include<Util/StringUtil.h>
using namespace std;
using namespace Common;

namespace JgAsset
{
	namespace AssetManager
	{
		std::unordered_map<std::string, AssetMeshStream>          g_MeshStreams;
		std::unordered_map<std::string, AssetSkeletalMeshStream>  g_SkMeshStreams;
		std::unordered_map<std::string, AssetBoneHierarchyStream> g_BoneHichyStreams;
		std::unordered_map<std::string, AssetAnimationStream>     g_AnimStreams;


		bool LoadMesh(AssetMeshStream& stream, const string& path)
		{
			if (Util::Path_ExtractFileFormat(path) != ASSET_MESH_FILEFORMAT)
				return false;
			if (g_MeshStreams.find(path) != g_MeshStreams.end())
			{
				stream = g_MeshStreams[path];
				return true;
			}
			

			Data_IO fin(EIOType::Read);
			IO_Result result = fin.Read(path);

			if (result != Data_IO::IO_Result_Success)
				return false;

			fin >> stream;

			g_MeshStreams[path] = stream;
			return true;
		}

		bool LoadSkeletalMesh(AssetSkeletalMeshStream& stream, const string& path)
		{
			if (Util::Path_ExtractFileFormat(path) != ASSET_SKELETALMESH_FILEFORMAT)
				return false;
			if (g_SkMeshStreams.find(path) != g_SkMeshStreams.end())
			{
				stream = g_SkMeshStreams[path];
				return true;
			}
			Data_IO fin(EIOType::Read);
			IO_Result result = fin.Read(path);

			if (result != Data_IO::IO_Result_Success)
				return false;

			fin >> stream;

			g_SkMeshStreams[path] = stream;
			return true;
		}
		bool LoadSkeletal(AssetBoneHierarchyStream& stream, const string& path)
		{
			if (Util::Path_ExtractFileFormat(path) != ASSET_SKELETALHIERARCHY_FILEFORMAT)
				return false;
			if (g_BoneHichyStreams.find(path) != g_BoneHichyStreams.end())
			{
				stream = g_BoneHichyStreams[path];
				return true;
			}
			Data_IO fin(EIOType::Read);
			IO_Result result = fin.Read(path);

			if (result != Data_IO::IO_Result_Success)
				return false;

			fin >> stream;

			g_BoneHichyStreams[path] = stream;
			return true;
		}
		bool LoadAnimation(AssetAnimationStream& stream, const string& path)
		{
			if (Util::Path_ExtractFileFormat(path) != ASSET_ANIM_FILEFORMAT)
				return false;

			if (g_AnimStreams.find(path) != g_AnimStreams.end())
			{
				stream = g_AnimStreams[path];
				return true;
			}
			Data_IO fin(EIOType::Read);
			IO_Result result = fin.Read(path);

			if (result != Data_IO::IO_Result_Success)
				return false;

			fin >> stream;

			g_AnimStreams[path] = stream;
			return true;
		}


		bool CreateAnimationTransformsByTimePos(
			AssetAnimTransformByTimePos& Output,
			const std::string& skMeshPath,
			const std::string& animPath,
			const std::string& skeletalPath)
		{
			AssetSkeletalMeshStream skMesh;
			AssetBoneHierarchyStream skeletal;
			AssetAnimationStream anim;
			if (Util::Path_ExtractFileFormat(animPath) == ASSET_ANIM_FILEFORMAT)
			{
				if (!LoadAnimation(anim, animPath))
					return false;
			}
			else
				return false;

			if (Util::Path_ExtractFileFormat(skMeshPath) == ASSET_SKELETALMESH_FILEFORMAT)
			{
				if (!LoadSkeletalMesh(skMesh, skMeshPath))
					return false;
			}
			else
				return false;
			if (Util::Path_ExtractFileFormat(skeletalPath) == ASSET_SKELETALHIERARCHY_FILEFORMAT)
			{
				if (!LoadSkeletal(skeletal, skeletalPath))
					return false;
			}
			else
				return false;


			BonesByID boneByNodeID;
			for (auto& boneNode : skeletal.Bones)
			{
				AssetBone bone;
				bone.NodeData = boneNode;
				boneByNodeID[boneNode.ID] = bone;
			}

			uint32_t boneCount = (uint32_t)skMesh.BoneOffsets.size();
			for (auto& bone : boneByNodeID)
			{
				for (auto iter = skMesh.BoneOffsets.begin(); iter < skMesh.BoneOffsets.end();)
				{
					if (iter->Name == bone.second.NodeData.Name)
					{
						bone.second.OffsetData = *iter;
						iter = skMesh.BoneOffsets.erase(iter);
						break;
					}
					else
					{
						iter++;
					}
						
				}
			}
			if (!skMesh.BoneOffsets.empty())
				return false;


			for (uint32_t i = 0; i < (uint32_t)anim.Duration; ++i)
			{
				float time = (float)i;
				AssetAnimTransform transform;
				transform.Create(time, &boneByNodeID, &anim, boneCount);
				Output[time] = transform;
			}

			return true;
		}
	}
}
