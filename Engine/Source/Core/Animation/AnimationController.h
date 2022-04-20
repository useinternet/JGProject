#pragma once

#include "JGCore.h"

namespace JG
{
	class AnimationClip;
	class AnimationClipInfo;
	class AnimationSequence;
	class AnimationTransition;
	class AnimationParameters;

	class AnimationController
	{
		// Ŭ�� �� �߰��Ǹ� AnimationClipInfo ����

		SharedPtr<AnimationSequence>   mRootSequence;
		SharedPtr<AnimationParameters> mAnimParams;
	public:
		// Clip ã��
		void AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip);
		void RemoveAnimationClip(const String& name);

		SharedPtr<AnimationClip>	 FindAnimationClip(const String& name) const;
		SharedPtr<AnimationClipInfo> FindAnimationClipInfo(const String& name);


		SharedPtr<AnimationParameters> GetAnimationParameters() const;
		SharedPtr<AnimationSequence> GetRootAnimationSequence() const;
	};
}