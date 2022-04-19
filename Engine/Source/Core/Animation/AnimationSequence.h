#pragma once
#include "JGCore.h"

namespace JG
{
	class AnimationController;
	class AnimationTransition;


	class AnimationSequenceNexus
	{
	private:
		AnimationSequence* Pointer = nullptr;
	public:
		AnimationSequenceNexus(AnimationSequence* Pointer) : Pointer(Pointer) {}


		//AnimationSequenceNexus MakeSequence();
		//AnimationSequenceNexus MakeAnimationClip();
		//AnimationSequenceNexus ConnectSequence();
		//AnimationSequenceNexus ConnectAnimationClip();
		//void End();
	};

	class AnimationSequence
	{
		WeakPtr<AnimationController> mOwnerAnimController;
	public:
		bool IsValid() const;
		SharedPtr<AnimationController> GetOwnerAnimationController() const;

		AnimationSequenceNexus Begin();
		AnimationSequenceNexus MakeSequence();
		AnimationSequenceNexus MakeAnimationClip();
		AnimationSequenceNexus ConnectSequence();
		AnimationSequenceNexus ConnectAnimationClip();
		void End();
	};
}