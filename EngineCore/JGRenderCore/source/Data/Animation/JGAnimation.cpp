#include"JGAnimation.h"
#include"Data/Mesh/JGSkeletalMesh.h"
#include"DxCommon/ModelLoader.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;

vector<XMFLOAT4X4> JGAnimation::UpdateAnimation(float TimePos, JGBoneNode* Root, UINT BoneCount)
{
	vector<XMFLOAT4X4> resultTransform(BoneCount);
	for (auto& mat : resultTransform)
		mat = MathHelper::Identity4x4();
	XMFLOAT4X4 Mat = MathHelper::Identity4x4();
	ReadAnim(TimePos, Root, resultTransform, Mat);

	return resultTransform;
}
void JGAnimation::ReadAnim(
	float               TimePos,
	JGBoneNode*         node,
	vector<XMFLOAT4X4>& finalTransform,
	XMFLOAT4X4&         ParentMat)
{
	JGAnimChannel* AnimChannel = FindAnimChannel(node);
	XMFLOAT4X4 NodeTransform = node->Transform;

	if (AnimChannel)
	{
		XMFLOAT3 Translation = { 0.0f,0.0f,0.0f };
		CalcPositionFrame(TimePos, AnimChannel, Translation);
		XMVECTOR TransVec = XMLoadFloat3(&Translation);
		

		XMFLOAT3 Scale = { 1.0f,1.0f,1.0f };
		CalcScaleFrame(TimePos, AnimChannel, Scale);
		XMVECTOR ScaleVec = XMLoadFloat3(&Scale);


		XMFLOAT4 Rotation = { 0.0f,0.0f,0.0f,0.0f };
		CalcQuatFrame(TimePos, AnimChannel, Rotation);
		XMVECTOR QuatVec = XMLoadFloat4(&Rotation);


		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&NodeTransform, XMMatrixAffineTransformation(ScaleVec, zero, QuatVec, TransVec));
	}
	XMMATRIX XMNodeTransform   = XMLoadFloat4x4(&NodeTransform);
	XMMATRIX XMParentTransform = XMLoadFloat4x4(&ParentMat);
	XMMATRIX XMResultTransform = XMMatrixMultiply(XMNodeTransform, XMParentTransform);
	
	XMStoreFloat4x4(&NodeTransform, XMResultTransform);


	if (node->bExist)
	{
		XMMATRIX OffsetTransform = XMLoadFloat4x4(&node->Data.Offset);
		XMMATRIX InvGobalTransform = XMLoadFloat4x4(&InvGlobalTransform);
		XMStoreFloat4x4(&finalTransform[node->Data.Index],
			XMMatrixTranspose(
				XMMatrixMultiply(InvGobalTransform, 
					             XMMatrixMultiply(OffsetTransform, XMResultTransform))));
	}
	for (UINT i = 0; i < node->Child.size(); ++i)
	{
		ReadAnim(TimePos, node->Child[i], finalTransform, NodeTransform);
	}
}
JGAnimChannel* JGAnimation::FindAnimChannel(JGBoneNode* node)
{
	for (auto& channel : AnimChannels)
	{
		if (channel.Name == node->Data.Name)
		{
			return &channel;
		}
	}

	return nullptr;
}
void JGAnimation::CalcPositionFrame(float TimePos, JGAnimChannel* channel, XMFLOAT3& pos)
{
	if (channel->AnimPositionFrames.size() == 1)
	{
		pos = channel->AnimPositionFrames[0].Value;
		return;
	}
	UINT PosIndex = FindPositionIndex(TimePos, channel);
	UINT NextPosIndex = PosIndex + 1;
	assert(NextPosIndex < channel->AnimPositionFrames.size());
	//
	float DeltaTime = channel->AnimPositionFrames[NextPosIndex].TimePos - channel->AnimPositionFrames[PosIndex].TimePos;
	float InterpFactor = (TimePos - channel->AnimPositionFrames[PosIndex].TimePos) / DeltaTime;
	assert(InterpFactor >= 0.0f && InterpFactor <= 1.0f);
	// 

	XMVECTOR Start = XMLoadFloat3(&channel->AnimPositionFrames[PosIndex].Value);
	XMVECTOR End = XMLoadFloat3(&channel->AnimPositionFrames[NextPosIndex].Value);
	XMVECTOR Result = XMVectorLerp(Start, End, InterpFactor);
	XMStoreFloat3(&pos, Result);
}
void JGAnimation::CalcScaleFrame(float TimePos, JGAnimChannel* channel, XMFLOAT3& scale)
{
	if (channel->AnimScaleFrames.size() == 1)
	{
		scale = channel->AnimScaleFrames[0].Value;
		return;
	}
	UINT ScaleIndex = FindScaleIndex(TimePos, channel);
	UINT NextScaleIndex = ScaleIndex + 1;
	assert(NextScaleIndex < channel->AnimScaleFrames.size());
	//
	float DeltaTime = channel->AnimScaleFrames[NextScaleIndex].TimePos - channel->AnimScaleFrames[ScaleIndex].TimePos;
	float InterpFactor = (TimePos - channel->AnimScaleFrames[ScaleIndex].TimePos) / DeltaTime;
	assert(InterpFactor >= 0.0f && InterpFactor <= 1.0f);
	// 

	XMVECTOR Start = XMLoadFloat3(&channel->AnimScaleFrames[ScaleIndex].Value);
	XMVECTOR End = XMLoadFloat3(&channel->AnimScaleFrames[NextScaleIndex].Value);
	XMVECTOR Result = XMVectorLerp(Start, End, InterpFactor);

	XMStoreFloat3(&scale, Result);
}
void JGAnimation::CalcQuatFrame(float TimePos, JGAnimChannel* channel, XMFLOAT4& quat)
{
	if (channel->AnimRotationFrames.size() == 1)
	{
		quat = channel->AnimRotationFrames[0].Value;
		return;
	}
	UINT QuatIndex = FindQuatIndex(TimePos, channel);
	UINT NextQuatIndex = QuatIndex + 1;
	assert(NextQuatIndex < channel->AnimRotationFrames.size());

	//
	float DeltaTime = channel->AnimRotationFrames[NextQuatIndex].TimePos - channel->AnimRotationFrames[QuatIndex].TimePos;
	float InterpFactor = (TimePos - channel->AnimRotationFrames[QuatIndex].TimePos) / DeltaTime;
	assert(InterpFactor >= 0.0f && InterpFactor <= 1.0f);

	XMVECTOR Start = XMLoadFloat4(&channel->AnimRotationFrames[QuatIndex].Value);
	XMVECTOR End = XMLoadFloat4(&channel->AnimRotationFrames[NextQuatIndex].Value);
	XMVECTOR Result = XMQuaternionSlerp(Start, End, InterpFactor);
	XMStoreFloat4(&quat, Result);
}


UINT JGAnimation::FindPositionIndex(float TimePos, JGAnimChannel* channel)
{
	UINT len = (UINT)channel->AnimPositionFrames.size();
	for (UINT i = 0; i < len - 1; ++i)
	{
		if (TimePos < channel->AnimPositionFrames[i + 1].TimePos)
			return i;
	}
	assert(0);
	return 0;
}
UINT JGAnimation::FindScaleIndex(float TimePos, JGAnimChannel* channel)
{
	UINT len = (UINT)channel->AnimScaleFrames.size();
	for (UINT i = 0; i < len - 1; ++i)
	{
		if (TimePos < channel->AnimScaleFrames[i + 1].TimePos)
			return i;
	}
	assert(0);
	return 0;
}
UINT JGAnimation::FindQuatIndex(float TimePos, JGAnimChannel* channel)
{
	UINT len = (UINT)channel->AnimRotationFrames.size();
	for (UINT i = 0; i < len - 1; ++i)
	{
		if (TimePos < channel->AnimRotationFrames[i + 1].TimePos)
			return i;
	}
	assert(0);
	return 0;
}



unordered_map<AnimTransKeyPack, vector<vector<XMFLOAT4X4>>, AnimTransKeyPackHash> JGAnimationHelper::AnimationTransforms;
void JGAnimationHelper::BuildAnimationData(JGAnimation* anim, JGBoneNode* Root, UINT BoneCount, const string& meshName)
{
	AnimTransKeyPack  Key = { anim, Root, BoneCount };
	if (AnimationTransforms.find(Key) != AnimationTransforms.end())
	{
		AnimTransform = AnimationTransforms[Key];
		return;
	}
	float duration = anim->Duration;
	AnimTransform.resize((UINT)duration);
	for (UINT time = 0; time < duration; ++time)
	{
		AnimTransform[time] = move(anim->UpdateAnimation((float)time, Root, BoneCount));
	}
	AnimationTransforms[Key] = AnimTransform;
}
void JGAnimationHelper::UpdateAnimatoin(const GameTimer& gt, JGAnimation* anim, JGBoneNode* Root, UINT BoneCount)
{
	if (gt.DeltaTime() < 0.0f)
		return;
	IsPlay = true;


	TimePos += ( gt.DeltaTime() * anim->TickPerSecond);
	

	if (TimePos > anim->Duration)
		TimePos = 0.0f;

	
	FinalTransform = AnimTransform[(UINT)TimePos];
}
SkinnedConstantData JGAnimationHelper::Get()
{
	SkinnedConstantData data;
	copy(FinalTransform.begin(), FinalTransform.end(), &data.BoneTransforms[0]);
	return data;
}