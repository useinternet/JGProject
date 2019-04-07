#pragma once

#include<DirectXMath.h>
#include<vector>
#include<fstream>
#include<string>
#include<unordered_map>
#include<map>
#pragma comment(lib, "assimp-vc140-mt.lib")
#define ASSET_MESH_FILEFORMAT ".mesh"
#define ASSET_SKELETALMESH_FILEFORMAT ".skmesh"
#define ASSET_SKELETALHIERARCHY_FILEFORMAT ".skeletal"
#define ASSET_ANIM_FILEFORMAT ".anim"

namespace Common
{
	class Data_IO;
}
namespace JgAsset
{
	struct AssetBone;
	struct AssetBoneOffset;
	struct AssetAnimation;
	class  AssetAnimTransform;

	typedef uint32_t                                AssetIndex;
	typedef std::map<uint32_t, AssetBone> BonesByID;
	typedef std::vector<AssetBoneOffset>            AssetBoneOffsets;
	typedef AssetAnimation                          AssetAnimationStream;
	typedef std::map<float, AssetAnimTransform>     AssetAnimTransformByTimePos;
	typedef std::vector<DirectX::XMFLOAT4X4>        AssetAnimTransformData;
	// 에셋 타입
	enum EAssetTypes
	{
		AssetType_Mesh         = 0x01,
		AssetType_Animation    = 0x02,
		AssetType_SkeletalMesh = 0x04,
		AssetType_BoneHierarchy= 0x08
	};
	EAssetTypes operator|(EAssetTypes type1, EAssetTypes type2);



	// 정점 정보 류

	struct AssetVertex
	{
		DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 normal   = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 tangent  = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT2 texcoord = { 0.0f,0.0f };
	};
	struct AssetSkeletalVertex
	{
		DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 normal = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 tangent = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT2 texcoord = { 0.0f,0.0f };
		float boneWeights[4] = { 0.0f,0.0f,0.0f,0.0f };
		AssetIndex bondId[4]   = { 0,0,0,0 };
	};
	struct AssetVectorFrame
	{
		float timepos;
		DirectX::XMFLOAT3 value;
	};
	struct AssetQuatFrame
	{
		float timepos;
		DirectX::XMFLOAT4 value;
	};

	// 메시 류
	struct AssetMesh
	{
		std::vector<AssetVertex> AssetVertexs;
		std::vector<AssetIndex>  AssetIndices;
		bool IsTangents  = false;
		bool IsTexcoords = false;
	};
	struct AssetSkeletalMesh
	{
		std::vector<AssetSkeletalVertex> AssetVertexs;
		std::vector<AssetIndex>          AssetIndices;
		bool IsTangents = false;
		bool IsTexcoords = false;
	};
	struct AssetBoneNode
	{
		std::string Name;
		int ID;
		DirectX::XMFLOAT4X4 Transform;
		int  ParentID;
		uint32_t ChildCount;
		std::vector<int> ChildIDs;
	
		AssetBoneNode() {
			DirectX::XMStoreFloat4x4(&Transform, DirectX::XMMatrixIdentity());
			ChildCount = 0;
			ID = -1;
			ParentID = -1;
		}
	};

	struct AssetBoneOffset
	{
		std::string Name;
		int ID;
		DirectX::XMFLOAT4X4 Offset;
		AssetBoneOffset() {
			DirectX::XMStoreFloat4x4(&Offset, DirectX::XMMatrixIdentity());
			ID = -1;
		}
	};
	struct AssetBone
	{
		AssetBoneNode   NodeData;
		AssetBoneOffset OffsetData;
	};
	struct AssetAnimChannel
	{
		std::string Name;
		std::vector<AssetVectorFrame> AnimPositionFrames;
		std::vector<AssetVectorFrame> AnimScaleFrames;
		std::vector<AssetQuatFrame>   AnimRotationFrames;
	};
	struct AssetAnimation
	{
		std::string Name;
		std::vector<AssetAnimChannel> AnimChannels;
		DirectX::XMFLOAT4X4           InvGlobalTransform;
		float Duration      = 0.0f;
		float TickPerSecond = 0.0f;
	};

	// 패키지 류
	struct AssetMeshPackage
	{
		std::vector<std::string> AssetMeshNames;
		std::vector<AssetMesh>   AssetMeshs;
	};
	struct AssetBoneHierarchyPackage
	{
		std::string Name;
		std::vector<AssetBoneNode> BoneNodes;
	};


	struct AssetSkeletalMeshPackage
	{
		std::vector<std::string>       AssetMeshNames;
		std::vector<AssetSkeletalMesh> AssetMeshs;
		std::vector<AssetBoneOffsets>  AssetBoneOffsets;
	};

	// 스트림 류
	struct AssetMeshStream
	{
	    std::string MeshName;
		AssetMesh   MeshData;
	};
	struct AssetSkeletalMeshStream
	{
		std::string MeshName;
		AssetSkeletalMesh MeshData;
		AssetBoneOffsets  BoneOffsets;
	};
	struct AssetBoneHierarchyStream
	{
		std::string Name;
		std::vector<AssetBoneNode> Bones;
	};
}

Common::Data_IO& operator<<(Common::Data_IO& io, JgAsset::AssetMesh& stream);
Common::Data_IO& operator>>(Common::Data_IO& io, JgAsset::AssetMesh& stream);
Common::Data_IO& operator<<(Common::Data_IO& io, JgAsset::AssetSkeletalMesh& stream);
Common::Data_IO& operator>>(Common::Data_IO& io, JgAsset::AssetSkeletalMesh& stream);
Common::Data_IO& operator<<(Common::Data_IO& io, JgAsset::AssetBoneOffset& stream);
Common::Data_IO& operator>>(Common::Data_IO& io, JgAsset::AssetBoneOffset& stream);
Common::Data_IO& operator<<(Common::Data_IO& io, JgAsset::AssetBoneOffsets& stream);
Common::Data_IO& operator>>(Common::Data_IO& io, JgAsset::AssetBoneOffsets& stream);
Common::Data_IO& operator<<(Common::Data_IO& io, JgAsset::AssetBoneNode& stream);
Common::Data_IO& operator>>(Common::Data_IO& io, JgAsset::AssetBoneNode& stream);


Common::Data_IO& operator<<(Common::Data_IO& io, JgAsset::AssetMeshStream& stream);
Common::Data_IO& operator>>(Common::Data_IO& io, JgAsset::AssetMeshStream& stream);

Common::Data_IO& operator<<(Common::Data_IO& io, JgAsset::AssetSkeletalMeshStream& stream);
Common::Data_IO& operator>>(Common::Data_IO& io, JgAsset::AssetSkeletalMeshStream& stream);

Common::Data_IO& operator<<(Common::Data_IO& io, JgAsset::AssetBoneHierarchyStream& stream);
Common::Data_IO& operator>>(Common::Data_IO& io, JgAsset::AssetBoneHierarchyStream& stream);


Common::Data_IO& operator<<(Common::Data_IO& io, JgAsset::AssetAnimChannel& stream);
Common::Data_IO& operator>>(Common::Data_IO& io, JgAsset::AssetAnimChannel& stream);

Common::Data_IO& operator<<(Common::Data_IO& io, JgAsset::AssetAnimationStream& stream);
Common::Data_IO& operator>>(Common::Data_IO& io, JgAsset::AssetAnimationStream& stream);
