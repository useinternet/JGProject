#include "pch.h"
#include "AnimationTransition.h"


namespace JG
{



	void AnimationTransition::SetFlags(EAnimationTransitionFlags flags)
	{
		mFlags = flags;
	}

	void AnimationTransition::AddTransitionScript(SharedPtr<IAnimationTransitionScript> Script)
	{
		mScriptList.push_back(Script);
	}

	void AnimationTransition::AddCondition_Bool()
	{
	}

	void AnimationTransition::AddCondition_Int()
	{
	}

	void AnimationTransition::AddCondition_Float()
	{
	}

	void AnimationTransition::AddCondition_Vector2()
	{
	}

	void AnimationTransition::AddCondition_Vector3()
	{
	}

	void AnimationTransition::AddCondition_Vector4()
	{
	}

	bool AnimationTransition::Transition() const
	{


		//
		return false;
	}


}