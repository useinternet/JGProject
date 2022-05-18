#include "pch.h"
#include "AnimationBlendSpace1D.h"
#include "AnimationClip.h"
#include "AnimationTransform.h"
#include "Application.h"
#include "Class/Asset/Asset.h"
#include "Class/Data/Skeletone.h"
#include "Graphics/Mesh.h"

namespace JG
{
	void AnimationBlendSpace1DInfo::Reset()
	{

	}


	void AnimationBlendSpace1D::AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip)
	{


	}

	void AnimationBlendSpace1D::RemoveAnimationClip(const String& name)
	{
	}

	bool AnimationBlendSpace1D::GetCurrentKeyFrame(const String& nodeName, f32 timePos, JVector3* T, JQuaternion* Q, JVector3* S)
	{


		/*
					String currentNodeName = FindNodeName(node);
			const AnimationBlend1DInfo& _1dInfo = mAnimBlend1DInfoList[node->ID];

			f32 currValue = 0.0f;
			animController->GetAnimationParameters()->GetFloat(_1dInfo.ParamName, &currValue);

			String leftNodeName;
			String rightNodeName;

			f32 leftDist = JG_F32_MAX;
			f32 rightDist = JG_F32_MAX;

			u64 cnt = _1dInfo.ClipNameList.size();
			for (u64 i = 0; i < cnt; ++i)
			{
				f32 value = 0.0f;
				if (_1dInfo.Blend1DPosList[i].size() == sizeof(f32))
				{
					memcpy(&value, _1dInfo.Blend1DPosList[i].data(), sizeof(f32));
				}
				f32 dist = Math::Abs(currValue - value);
				// 현재값보다 작고, 현재값에서
				if (currValue >= value && dist < leftDist)
				{
					leftNodeName = _1dInfo.Name[i];
				}
				if (currValue < value && dist < rightDist)
				{
					rightNodeName = _1dInfo.Name[i];
				}
			}
			SharedPtr<AnimationClip>	 leftClip = animController->FindAnimationClip(leftClipName);
			SharedPtr<AnimationClipInfo> leftClipInfo = animController->FindAnimationClipInfo(leftClipName);

			SharedPtr<AnimationClip>	 rightClip = animController->FindAnimationClip(rightClipName);
			SharedPtr<AnimationClipInfo> rightClipInfo = animController->FindAnimationClipInfo(rightClipName);
			if (rightClip == nullptr && rightClipInfo == nullptr)
			{
				rightClip = leftClip;
				leftClipInfo = rightClipInfo;
			}
			f32 factor = leftDist / (rightDist + leftDist);

			JVector3 left_T; JQuaternion left_Q;
			JVector3 left_S;
			if (leftClip->GetCurrentKeyFrame(boneName, leftClipInfo->TimePos, &left_T, &left_Q, &left_S) == false)
			{
				return false;
			}
			JVector3 right_T; JQuaternion right_Q;
			JVector3 right_S;
			if (rightClip->GetCurrentKeyFrame(boneName, rightClipInfo->TimePos, &right_T, &right_Q, &right_S) == false)
			{
				return false;
			}

			if (T) *T = JVector3::Lerp(left_T, right_T, factor);
			if (Q) *Q = JQuaternion::Slerp(left_Q, right_Q, factor);
			if (S) *S = JVector3::Lerp(left_S, right_S, factor);
		*/
		return false;
	}

	bool AnimationBlendSpace1D::IsValid() const
	{
		return false;
	}

	void AnimationBlendSpace1D::SetAnimationBlendSpace1DStock(const AnimationBlendSpace1DStock& stock)
	{
	}

	void AnimationBlendSpace1D::SetName(const String& name)
	{
		mName = name;
	}

	const String& AnimationBlendSpace1D::GetName() const
	{
		return mName;
	}

	SharedPtr<AnimationBlendSpace1D> AnimationBlendSpace1D::Create(const String& name)
	{
		return SharedPtr<AnimationBlendSpace1D>();
	}

	SharedPtr<AnimationBlendSpace1D> AnimationBlendSpace1D::Create(const AnimationBlendSpace1DStock& stock)
	{
		return SharedPtr<AnimationBlendSpace1D>();
	}

}