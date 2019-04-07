#include<PCH.h>
#include"AssetAnimTransform.h"
using namespace JgAsset;
using namespace DirectX;
using namespace std;

void AssetAnimTransform::Create(float TimePos, BonesByID* BonesByNodeID,
	AssetAnimation* pAnim, uint32_t BoneCount)
{
	m_pBonesByNodeID   = BonesByNodeID;
	m_pAnim = pAnim;
	m_BoneCount = BoneCount;
	XMFLOAT4X4 initMat;
	XMStoreFloat4x4(&initMat, XMMatrixIdentity());

	m_Matrixs.resize(m_BoneCount, initMat);
	ProcessCalcAnimTransform(TimePos, 0, initMat, BonesByNodeID);
}

void AssetAnimTransform::ProcessCalcAnimTransform(
	float TimePos,
	uint32_t NodeID,
	XMFLOAT4X4& ParentMat,
	BonesByID* BonesByNodeID)
{
	AssetBone* CurrBone = &m_pBonesByNodeID->at(NodeID);
	AssetAnimChannel* channel = FindAnimChannel(CurrBone->NodeData.Name);
	XMFLOAT4X4 NodeTransform = CurrBone->NodeData.Transform;

	if (channel)
	{
		XMFLOAT3 translation = { 0.0f,0.0f,0.0f };
		CalcPositionFrame(TimePos, channel, translation);
		XMVECTOR transVec = XMLoadFloat3(&translation);


		XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
		CalcScaleFrame(TimePos , channel, scale);
		XMVECTOR scaleVec = XMLoadFloat3(&scale);

		XMFLOAT4 rotation = { 0.0f,0.0f,0.0f,0.0f };
		CalcRotationFrame(TimePos, channel, rotation);
		XMVECTOR QuatVec = XMLoadFloat4(&rotation);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMStoreFloat4x4(&NodeTransform,
			XMMatrixAffineTransformation(scaleVec, zero, QuatVec, transVec));
	}

	XMMATRIX resultTransform =
		XMMatrixMultiply(XMLoadFloat4x4(&NodeTransform), XMLoadFloat4x4(&ParentMat));
	XMStoreFloat4x4(&NodeTransform, resultTransform);

	if (CurrBone->OffsetData.ID != -1)
	{
		resultTransform =
			XMMatrixMultiply(XMLoadFloat4x4(&CurrBone->OffsetData.Offset), resultTransform);
		resultTransform =
			XMMatrixMultiply(XMLoadFloat4x4(&m_pAnim->InvGlobalTransform), resultTransform);
		XMStoreFloat4x4(&m_Matrixs[CurrBone->OffsetData.ID], XMMatrixTranspose(resultTransform));
	}

	for (uint32_t i = 0; i < CurrBone->NodeData.ChildCount; ++i)
	{
		ProcessCalcAnimTransform(TimePos,
			CurrBone->NodeData.ChildIDs[i], NodeTransform, BonesByNodeID);
	}
}

void AssetAnimTransform::CalcPositionFrame(float TimePos, AssetAnimChannel* channel, XMFLOAT3& pos)
{
	if (channel->AnimPositionFrames.size() == 1)
	{
		pos = channel->AnimPositionFrames[0].value;
		return;
	}
	uint32_t posIndex = FindPositionIndex(TimePos, channel);
	uint32_t nextPosIndex = posIndex + 1;
	assert(nextPosIndex < channel->AnimPositionFrames.size());

	//

	float deltaTime = channel->AnimPositionFrames[nextPosIndex].timepos - channel->AnimPositionFrames[posIndex].timepos;
	float interpFactor = (TimePos - channel->AnimPositionFrames[posIndex].timepos) / deltaTime;
	assert(interpFactor >= 0.0f && interpFactor <= 1.0f);
	//
	XMVECTOR Start = XMLoadFloat3(&channel->AnimPositionFrames[posIndex].value);
	XMVECTOR End = XMLoadFloat3(&channel->AnimPositionFrames[nextPosIndex].value);
	XMVECTOR Result = XMVectorLerp(Start, End, interpFactor);

	XMStoreFloat3(&pos, Result);
}
void AssetAnimTransform::CalcScaleFrame(float TimePos, AssetAnimChannel* channel, XMFLOAT3& scale)
{
	if (channel->AnimScaleFrames.size() == 1)
	{
		scale = channel->AnimScaleFrames[0].value;
		return;
	}
	uint32_t ScaleIndex = FindScaleIndex(TimePos, channel);
	uint32_t NextScaleIndex = ScaleIndex + 1;
	assert(NextScaleIndex < channel->AnimScaleFrames.size());
	//
	float DeltaTime = channel->AnimScaleFrames[NextScaleIndex].timepos - channel->AnimScaleFrames[ScaleIndex].timepos;
	float InterpFactor = (TimePos - channel->AnimScaleFrames[ScaleIndex].timepos) / DeltaTime;
	assert(InterpFactor >= 0.0f && InterpFactor <= 1.0f);
	// 

	XMVECTOR Start = XMLoadFloat3(&channel->AnimScaleFrames[ScaleIndex].value);
	XMVECTOR End = XMLoadFloat3(&channel->AnimScaleFrames[NextScaleIndex].value);
	XMVECTOR Result = XMVectorLerp(Start, End, InterpFactor);

	XMStoreFloat3(&scale, Result);
}
void AssetAnimTransform::CalcRotationFrame(float TimePos, AssetAnimChannel* channel, XMFLOAT4& quat)
{
	if (channel->AnimRotationFrames.size() == 1)
	{
		quat = channel->AnimRotationFrames[0].value;
		return;
	}
	uint32_t RotationIndex = FindRotationIndex(TimePos, channel);
	uint32_t NextRotationIndex = RotationIndex + 1;
	assert(NextRotationIndex < channel->AnimRotationFrames.size());

	//
	float DeltaTime = channel->AnimRotationFrames[NextRotationIndex].timepos - channel->AnimRotationFrames[RotationIndex].timepos;
	float InterpFactor = (TimePos - channel->AnimRotationFrames[RotationIndex].timepos) / DeltaTime;
	assert(InterpFactor >= 0.0f && InterpFactor <= 1.0f);

	XMVECTOR Start = XMLoadFloat4(&channel->AnimRotationFrames[RotationIndex].value);
	XMVECTOR End = XMLoadFloat4(&channel->AnimRotationFrames[NextRotationIndex].value);
	XMVECTOR Result = XMQuaternionSlerp(Start, End, InterpFactor);
	XMStoreFloat4(&quat, Result);
}

uint32_t AssetAnimTransform::FindPositionIndex(float TimePos, AssetAnimChannel* channel)
{
	uint32_t len = (uint32_t)channel->AnimPositionFrames.size();

	for (uint32_t i = 0; i < len - 1; ++i)
	{
		if (TimePos < channel->AnimPositionFrames[i + 1].timepos)
			return i;
	}
	assert(0);
	return 0;
}
uint32_t AssetAnimTransform::FindScaleIndex(float TimePos, AssetAnimChannel* channel)
{
	uint32_t len = (uint32_t)channel->AnimScaleFrames.size();
	for (uint32_t i = 0; i < len - 1; ++i)
	{
		if (TimePos < channel->AnimScaleFrames[i + 1].timepos)
			return i;
	}
	assert(0);
	return 0;
}
uint32_t AssetAnimTransform::FindRotationIndex(float TimePos, AssetAnimChannel* channel)
{
	uint32_t len = (uint32_t)channel->AnimRotationFrames.size();
	for (uint32_t i = 0; i < len - 1; ++i)
	{
		if (TimePos < channel->AnimRotationFrames[i + 1].timepos)
			return i;
	}
	assert(0);
	return 0;
}

AssetAnimChannel* AssetAnimTransform::FindAnimChannel(const std::string& NodeName)
{
	for (auto& channel : m_pAnim->AnimChannels)
	{
		if (channel.Name == NodeName)
			return &channel;
	}
	return nullptr;
}