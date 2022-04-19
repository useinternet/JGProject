#include "pch.h"
#include "AnimationSequence.h"


namespace JG
{
	bool AnimationSequence::IsValid() const
	{
		return false;
	}

	SharedPtr<AnimationController> AnimationSequence::GetOwnerAnimationController() const
	{
		return SharedPtr<AnimationController>();
	}

	AnimationSequenceNexus AnimationSequence::Begin()
	{
		return AnimationSequenceNexus(this);
	}

	AnimationSequenceNexus AnimationSequence::MakeSequence()
	{
		return AnimationSequenceNexus(this);
	}

	AnimationSequenceNexus AnimationSequence::MakeAnimationClip()
	{
		return AnimationSequenceNexus(this);
	}

	AnimationSequenceNexus AnimationSequence::ConnectSequence()
	{
		return AnimationSequenceNexus(this);
	}

	AnimationSequenceNexus AnimationSequence::ConnectAnimationClip()
	{
		return AnimationSequenceNexus(this);
	}

	void AnimationSequence::End()
	{
	}

}