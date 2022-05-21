#include "pch.h"
#include "AnimationClip.h"
#include "AnimationTransform.h"
#include "Application.h"
#include "Class/Asset/Asset.h"
#include "Class/Data/Skeletone.h"
#include "Graphics/Mesh.h"

namespace JG
{
	AnimationClipInfo::AnimationClipInfo(const String& name, SharedPtr<AnimationClip> clip, EAnimationClipFlags flags)
	{
		Name = name;
		TimePos = 0.0f;
		Duration = clip->GetDuration();
		Clip = clip;
		Flags = flags;
		TickPerSecond = clip->GetTickPerSecond();
	}

	void AnimationClipInfo::Update(f32 tick, f32 multiplier)
	{
		TimePos += (tick * TickPerSecond * Speed * 10 * multiplier);
		if (Flags & EAnimationClipFlags::Repeat)
		{
			Reset();
		}
	}

	bool AnimationClip::IsValid() const
	{
		return mDuration > 0;
	}

	void AnimationClip::SetAnimationClipStock(const AnimationClipAssetStock& stock)
	{
		mClipName = stock.Name;
		mDuration = stock.Duration;
		mTickPerSecond = stock.TicksPerSecond;

		for (const auto& _pair : stock.AnimationNodes)
		{
			AnimationNode node;
			node.NodeName = _pair.second.NodeName;
			node.LocationValues = _pair.second.LocationValues;
			node.LocationTimes = _pair.second.LocationTimes;
			node.RotationValues = _pair.second.RotationValues;
			node.RotationTimes = _pair.second.RotationTimes;
			node.ScaleValues = _pair.second.ScaleValues;
			node.ScaleTimes = _pair.second.ScaleTimes;
			mAnimationNodes.emplace(_pair.first, node);
		}
	}

	void AnimationClip::SetName(const String& name)
	{
		mName = name;
	}

	const String& AnimationClip::GetName() const
	{
		return mName;
	}

	const String& AnimationClip::GetClipName() const
	{
		return mClipName;
	}

	f32 AnimationClip::GetDuration() const
	{
		return mDuration;
	}

	f32 AnimationClip::GetTickPerSecond() const
	{
		return mTickPerSecond;
	}

	bool AnimationClip::GetCurrentKeyFrame(const String& nodeName, const AnimationClipInfo& clipInfo, JVector3* T, JQuaternion* Q, JVector3* S)
	{
		const AnimationNode* animNode = FindAnimationNode(nodeName);
		if (animNode != nullptr)
		{
			if (T != nullptr)
			{
				*T = CalcLerpLocation(clipInfo.TimePos, animNode);
			}
			if (Q != nullptr)
			{
				*Q = CalcLerpRotation(clipInfo.TimePos, animNode);
			}
			if (S != nullptr)
			{
				*S = CalcLerpScale(clipInfo.TimePos, animNode);
			}
			return true;
		}
		return false;
	}


	JVector3 AnimationClip::CalcLerpLocation(f32 timePos, const AnimationNode* node)
	{
		if (node->LocationValues.size() == 1)
		{
			return node->LocationValues[0];
		}
		u32 LocationIndex = FindLocation(timePos, node);
		u32 NextLocationIndex = (LocationIndex + 1);
		if (NextLocationIndex == node->LocationTimes.size())
		{
			return node->LocationValues[LocationIndex];
		}

		f32 DeltaTime = node->LocationTimes[NextLocationIndex] - node->LocationTimes[LocationIndex];
		f32 Factor = (timePos - node->LocationTimes[LocationIndex]) / DeltaTime;


		JGASSERT(Factor >= 0.0f && Factor <= 1.0f);


		const JVector3& StartLocation = node->LocationValues[LocationIndex];
		const JVector3& EndLocation = node->LocationValues[NextLocationIndex];

		JVector3 Result = JVector3::Lerp(StartLocation, EndLocation, Factor);

		return Result;
	}
	JVector3 AnimationClip::CalcLerpScale(f32 timePos, const AnimationNode* node)
	{
		if (node->ScaleValues.size() == 1)
		{
			return node->ScaleValues[0];
		}
		u32 ScaleIndex = FindScale(timePos, node);
		u32 NextScaleIndex = (ScaleIndex + 1);
		if (NextScaleIndex == node->ScaleTimes.size())
		{
			return node->ScaleValues[ScaleIndex];
		}

		f32 DeltaTime = node->ScaleTimes[NextScaleIndex] - node->ScaleTimes[ScaleIndex];
		f32 Factor = (timePos - node->ScaleTimes[ScaleIndex]) / DeltaTime;


		JGASSERT(Factor >= 0.0f && Factor <= 1.0f);


		const JVector3& StartScale = node->ScaleValues[ScaleIndex];
		const JVector3& EndScale = node->ScaleValues[NextScaleIndex];

		JVector3 Result = JVector3::Lerp(StartScale, EndScale, Factor);

		return Result;
	}
	JQuaternion AnimationClip::CalcLerpRotation(f32 timePos, const  AnimationNode* node)
	{

		if (node->RotationValues.size() == 1) {
			return node->RotationValues[0];
		}

		u32 RotationIndex = FindRotation(timePos, node);
		u32 NextRotationIndex = (RotationIndex + 1);
		if (NextRotationIndex == node->RotationTimes.size())
		{
			return node->RotationValues[RotationIndex];
		}

		f32 DeltaTime = node->RotationTimes[NextRotationIndex] - node->RotationTimes[RotationIndex];
		f32 Factor = (timePos - node->RotationTimes[RotationIndex]) / DeltaTime;
		JGASSERT(Factor >= 0.0f && Factor <= 1.0f);
		const JQuaternion& StartRotationQ = node->RotationValues[RotationIndex];
		const JQuaternion& EndRotationQ    = node->RotationValues[NextRotationIndex];
		
		JQuaternion ResultQ  = JQuaternion::Slerp(StartRotationQ, EndRotationQ, Factor);
		ResultQ = JQuaternion::Normalize(ResultQ);

		return ResultQ;
	}
	u32 AnimationClip::FindRotation(f32 timePos, const AnimationNode* node)
	{
		for (u32 i = 0; i < node->RotationTimes.size() - 1; i++) {
			if (timePos < node->RotationTimes[i + 1]) {
				return i;
			}
		}
		return node->RotationTimes.size() - 1;
	}
	u32 AnimationClip::FindLocation(f32 timePos, const AnimationNode* node)
	{
		for (u32 i = 0; i < node->LocationTimes.size() - 1; i++) {
			if (timePos < node->LocationTimes[i + 1]) {
				return i;
			}
		}
		return node->LocationTimes.size() - 1;
	}
	u32 AnimationClip::FindScale(f32 timePos, const AnimationNode* node)
	{
		for (u32 i = 0; i < node->ScaleTimes.size() - 1; i++) {
			if (timePos < node->ScaleTimes[i + 1]) {
				return i;
			}
		}
		return node->ScaleTimes.size() - 1;
	}
	const AnimationClip::AnimationNode* AnimationClip::FindAnimationNode(const String& nodeName)
	{

		if (mAnimationNodes.find(nodeName) == mAnimationNodes.end())
		{
			return nullptr;
		}
		
		return &mAnimationNodes[nodeName];
	}

	SharedPtr<AnimationClip> AnimationClip::Create(const String& name)
	{
		SharedPtr<AnimationClip> animClip = CreateSharedPtr<AnimationClip>();
		animClip->SetName(name);
		return animClip;
	}

	SharedPtr<AnimationClip> AnimationClip::Create(const AnimationClipAssetStock& stock)
	{
		SharedPtr<AnimationClip> animClip = CreateSharedPtr<AnimationClip>();
		animClip->SetAnimationClipStock(stock);
		return animClip;
	}
}