#include "pch.h"
#include "AnimationController.h"
#include "AnimationClip.h"

namespace JG
{
	void AnimationController::AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, EAnimationClipFlags flags)
	{
		if (animationClip == nullptr || animationClip->IsValid() == false)
		{
			return;
		}
		CilpCommandData data;
		data.Command = CilpCommandData::Command_Add;
		data.Name = name;
		data.Clip = animationClip;
		data.ClipInfo = CreateSharedPtr<AnimationClipInfo>(name, animationClip, flags);
		mClipCommandDataQueue.push(data);
	}

	void AnimationController::RemoveAnimationClip(const String& name)
	{
		CilpCommandData data;
		data.Command = CilpCommandData::Command_Remove;
		data.Name = name;
		data.Clip     = nullptr;
		data.ClipInfo = nullptr;
		mClipCommandDataQueue.push(data);
	}

	void AnimationController::BindSkeletone(SharedPtr<Skeletone> skeletone)
	{
		mSkeletone = skeletone;
	}

	SharedPtr<AnimationClip> AnimationController::FindAnimationClip(const String& name) const
	{
		if (mAnimClips.find(name) == mAnimClips.end())
		{
			return nullptr;
		}
		return mAnimClips.at(name);
	}

	SharedPtr<AnimationClipInfo> AnimationController::FindAnimationClipInfo(const String& name)
	{
		if (mAnimClips.find(name) == mAnimClips.end())
		{
			return nullptr;
		}
		return mAnimClipInfos.at(name);
	}

	SharedPtr<Skeletone> AnimationController::GetBindedSkeletone() const
	{
		return mSkeletone;
	}

	SharedPtr<AnimationParameters> AnimationController::GetAnimationParameters() const
	{
		return mAnimParams;
	}

	SharedPtr<AnimationSequence> AnimationController::GetRootAnimationSequence() const
	{
		return mRootSequence;
	}



}