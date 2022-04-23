#pragma once

#include "JGCore.h"

namespace JG
{
	class AnimationClip;
	class AnimationClipInfo;
	class AnimationSequence;
	class AnimationTransition;
	class AnimationParameters;
	class Skeletone;
	class AnimationController
	{
		// 클립 이 추가되면 AnimationClipInfo 생성

		SharedPtr<AnimationSequence>   mRootSequence;
		SharedPtr<AnimationParameters> mAnimParams;
		SharedPtr<Skeletone> mSkeletone;
	public:
		// Clip 찾기
		void AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip);
		void RemoveAnimationClip(const String& name);

		SharedPtr<AnimationClip>	 FindAnimationClip(const String& name) const;
		SharedPtr<AnimationClipInfo> FindAnimationClipInfo(const String& name);

		SharedPtr<Skeletone> GetBindedSkeletone() const;
		SharedPtr<AnimationParameters> GetAnimationParameters() const;
		SharedPtr<AnimationSequence> GetRootAnimationSequence() const;
	};
}