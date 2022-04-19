#pragma once
#include "JGCore.h"
#include "AnimationDefines.h"


namespace JG
{
	class AnimationParameters;
	class AnimationClipInfo;
	class IAnimationTransitionScript
	{

	public:
		virtual bool Transition(
			SharedPtr<AnimationClipInfo> PrevClipInfo,
			SharedPtr<AnimationClipInfo> NextClipInfo,
			SharedPtr<AnimationParameters> AnimationParams) = 0;
	};

	class AnimationTransition
	{


	public:
		void AddTransitionScript(SharedPtr<IAnimationTransitionScript> Script);
	};
}