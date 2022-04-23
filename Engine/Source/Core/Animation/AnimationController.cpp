#include "pch.h"
#include "AnimationController.h"


namespace JG
{
	void AnimationController::AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip)
	{
	}

	void AnimationController::RemoveAnimationClip(const String& name)
	{
	}

	SharedPtr<AnimationClip> AnimationController::FindAnimationClip(const String& name) const
	{
		return SharedPtr<AnimationClip>();
	}

	SharedPtr<AnimationClipInfo> AnimationController::FindAnimationClipInfo(const String& name)
	{
		return SharedPtr<AnimationClipInfo>();
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