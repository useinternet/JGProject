#include<PCH.h>
#include"AssetDefine.h"
#include<Data/Data_IO.h>
#include<iostream>

using namespace JgAsset;
using namespace Common;
EAssetTypes JgAsset::operator|(EAssetTypes type1, EAssetTypes type2)
{
	return (EAssetTypes)((int)type1 | (int)type2);
}
Data_IO& operator<<(Data_IO& io, AssetMesh& stream)
{
	uint32_t VertexCount = (uint32_t)stream.AssetVertexs.size();
	uint32_t IndexCount = (uint32_t)stream.AssetIndices.size();

	io << stream.IsTangents;
	io << stream.IsTexcoords;

	io << VertexCount;
	io << IndexCount;

	io.DataPush(&stream.AssetVertexs[0], sizeof(AssetVertex) * VertexCount);
	io.DataPush(&stream.AssetIndices[0], sizeof(AssetIndex) * IndexCount);

	return io;
}
Data_IO& operator>>(Data_IO& io, AssetMesh& stream)
{
	uint32_t VertexCount;
	uint32_t IndexCount;
	io >> stream.IsTangents;
	io >> stream.IsTexcoords;

	io >> VertexCount;
	io >> IndexCount;
	//
	stream.AssetVertexs.resize(VertexCount);
	stream.AssetIndices.resize(IndexCount);
	//
	io.GetData(stream.AssetVertexs.data(), sizeof(AssetVertex) * VertexCount);
	io.GetData(stream.AssetIndices.data(), sizeof(AssetIndex) * IndexCount);

	return io;
}
Data_IO& operator<<(Data_IO& io, AssetSkeletalMesh& stream)
{
	uint32_t VertexCount = (uint32_t)stream.AssetVertexs.size();
	uint32_t IndexCount = (uint32_t)stream.AssetIndices.size();

	io << stream.IsTangents;
	io << stream.IsTexcoords;

	io << VertexCount;
	io << IndexCount;

	io.DataPush(&stream.AssetVertexs[0], sizeof(AssetSkeletalVertex) * VertexCount);
	io.DataPush(&stream.AssetIndices[0], sizeof(AssetIndex) * IndexCount);
	return io;
}
Data_IO& operator>>(Data_IO& io, AssetSkeletalMesh& stream)
{
	uint32_t VertexCount;
	uint32_t IndexCount;
	io >> stream.IsTangents;
	io >> stream.IsTexcoords;

	io >> VertexCount;
	io >> IndexCount;

	//
	stream.AssetVertexs.resize(VertexCount);
	stream.AssetIndices.resize(IndexCount);
	//
	io.GetData(stream.AssetVertexs.data(), sizeof(AssetSkeletalVertex) * VertexCount);
	io.GetData(stream.AssetIndices.data(), sizeof(AssetIndex) * IndexCount);

	return io;
}
Data_IO& operator<<(Data_IO& io, AssetBoneOffset& stream)
{
	io << stream.Name;
	io << stream.ID;
	io << stream.Offset;
	return io;
}
Data_IO& operator>>(Data_IO& io, AssetBoneOffset& stream)
{
	io >> stream.Name;
	io >> stream.ID;
	io >> stream.Offset;
	return io;
}
Data_IO& operator<<(Data_IO& io, AssetBoneOffsets& stream)
{
	uint32_t StreamSize = (uint32_t)stream.size();
	io << StreamSize;

	for (uint32_t i = 0; i < StreamSize; ++i)
	{
		io << stream[i];
	}

	return io;
}
Data_IO& operator>>(Data_IO& io, AssetBoneOffsets& stream)
{
	uint32_t StreamSize = 0;
	io >> StreamSize;

	stream.resize(StreamSize);
	for (uint32_t i = 0; i < StreamSize; ++i)
	{
		io >> stream[i];
	}
	return io;
}
Data_IO& operator<<(Data_IO& io, AssetBoneNode& stream)
{
	io << stream.Name;
	io << stream.ID;
	io << stream.Transform;
	io << stream.ParentID;
	io << stream.ChildCount;
	for (uint32_t i = 0; i < stream.ChildCount; ++i)
		io << stream.ChildIDs[i];

	return io;
}
Data_IO& operator>>(Data_IO& io, AssetBoneNode& stream)
{
	io >> stream.Name;
	io >> stream.ID;
	io >> stream.Transform;
	io >> stream.ParentID;
	io >> stream.ChildCount;
	stream.ChildIDs.resize(stream.ChildCount);

	for (uint32_t i = 0; i < stream.ChildCount; ++i)
		io >> stream.ChildIDs[i];
	return io;
}

Data_IO& operator<<(Data_IO& io, AssetMeshStream& stream)
{
	io << stream.MeshName;
	io << stream.MeshData;
	return io;
}

Data_IO& operator>>(Data_IO& io, AssetMeshStream& stream)
{
	io >> stream.MeshName;
	io >> stream.MeshData;

	return io;
}
Data_IO& operator<<(Data_IO& io, AssetSkeletalMeshStream& stream)
{
	io << stream.MeshName;
	io << stream.MeshData;
	io << stream.BoneOffsets;
	return io;
}
Data_IO& operator>>(Data_IO& io, AssetSkeletalMeshStream& stream)
{
	io >> stream.MeshName;
	io >> stream.MeshData;
	io >> stream.BoneOffsets;
	return io;
}
Data_IO& operator<<(Data_IO& io, AssetBoneHierarchyStream& stream)
{
	io << stream.Name;
	uint32_t streamSize = (uint32_t)stream.Bones.size();
	io << streamSize;
	for (uint32_t i = 0; i < streamSize; ++i)
	{
		io << stream.Bones[i];
	}
	return io;
}
Data_IO& operator>>(Data_IO& io, AssetBoneHierarchyStream& stream)
{
	io >> stream.Name;
	uint32_t streamSize;
	io >> streamSize;

	stream.Bones.resize(streamSize);

	for (uint32_t i = 0; i < streamSize; ++i)
	{
		io >> stream.Bones[i];
	}
	return io;
}

Data_IO& operator<<(Data_IO& io, AssetAnimChannel& stream)
{
	io << stream.Name;
	
	uint32_t posFrameCount      = (uint32_t)stream.AnimPositionFrames.size();
	uint32_t scaleFrameCount    = (uint32_t)stream.AnimScaleFrames.size();
	uint32_t rotationFrameCount = (uint32_t)stream.AnimRotationFrames.size();

	io << posFrameCount;
	io << scaleFrameCount;
	io << rotationFrameCount;

	io.DataPush(&stream.AnimPositionFrames[0], sizeof(stream.AnimPositionFrames[0]) * posFrameCount);
	io.DataPush(&stream.AnimScaleFrames[0], sizeof(stream.AnimScaleFrames[0]) * scaleFrameCount);
	io.DataPush(&stream.AnimRotationFrames[0], sizeof(stream.AnimRotationFrames[0]) * rotationFrameCount);
		

	return io;
}
Data_IO& operator>>(Data_IO& io, AssetAnimChannel& stream)
{
	io >> stream.Name;

	uint32_t posFrameCount, scaleFrameCount, rotationFrameCount;
	io >> posFrameCount;
	io >> scaleFrameCount;
	io >> rotationFrameCount;

	stream.AnimPositionFrames.resize(posFrameCount);
	stream.AnimScaleFrames.resize(scaleFrameCount);
	stream.AnimRotationFrames.resize(rotationFrameCount);

	io.GetData(&stream.AnimPositionFrames[0], sizeof(stream.AnimPositionFrames[0]) * posFrameCount);
	io.GetData(&stream.AnimScaleFrames[0], sizeof(stream.AnimScaleFrames[0]) * scaleFrameCount);
	io.GetData(&stream.AnimRotationFrames[0], sizeof(stream.AnimRotationFrames[0]) * rotationFrameCount);
	return io;
}

Data_IO& operator<<(Data_IO& io, AssetAnimationStream& stream)
{
	io << stream.Name;
	uint32_t ChannelCount = (uint32_t)stream.AnimChannels.size();
	io << ChannelCount;
	for (uint32_t i = 0; i < ChannelCount; ++i)
		io << stream.AnimChannels[i];
	io << stream.InvGlobalTransform;
	io << stream.Duration;
	io << stream.TickPerSecond;

	return io;
}
Data_IO& operator>>(Data_IO& io, AssetAnimationStream& stream)
{
	io >> stream.Name;
	uint32_t ChannelCount;
	io >> ChannelCount;
	
	stream.AnimChannels.resize(ChannelCount);
	for (uint32_t i = 0; i < ChannelCount; ++i)
		io >> stream.AnimChannels[i];

	io >> stream.InvGlobalTransform;
	io >> stream.Duration;
	io >> stream.TickPerSecond;
	return io;
}