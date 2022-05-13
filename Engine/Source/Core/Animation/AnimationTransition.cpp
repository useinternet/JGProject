#include "pch.h"
#include "AnimationTransition.h"
#include "AnimationParameters.h"

namespace JG
{



	void AnimationTransition::SetFlags(EAnimationTransitionFlags flags)
	{
		mFlags = flags;
	}

	void AnimationTransition::AddTransitionScript(SharedPtr<IAnimationTransitionScript> Script)
	{
		if (Script == nullptr)
		{
			return;
		}
		mTransitionInfoList.push_back(TransitionConditionInfo(mScriptTransitionList.size(), ETransitionType::Script));
		mScriptTransitionList.push_back(Script);
	}


	void AnimationTransition::AddCondition_Bool(const String& name, bool value)
	{
		mTransitionInfoList.push_back(TransitionConditionInfo(mBoolTransitionList.size(), ETransitionType::Bool));
		mBoolTransitionList.push_back(TransitionCondition<bool>(name, value, EAnimationCondition::Equal));
	}
	void AnimationTransition::AddCondition_Int(const String& name, i32 value, EAnimationCondition condition)
	{
		mTransitionInfoList.push_back(TransitionConditionInfo(mIntTransitionList.size(), ETransitionType::Int));
		mIntTransitionList.push_back(TransitionCondition<i32>(name, value, condition));
	}
	void AnimationTransition::AddCondition_Float(const String& name, f32 value, EAnimationCondition condition)
	{
		mTransitionInfoList.push_back(TransitionConditionInfo(mFloatTransitionList.size(), ETransitionType::Float));
		mFloatTransitionList.push_back(TransitionCondition<f32>(name, value, condition));
	}


	bool AnimationTransition::Transition() const
	{
		if (mTransitionInfoList.empty())
		{
			return true;
		}
		for (const TransitionConditionInfo& info : mTransitionInfoList)
		{
			switch (info.Type)
			{
			case ETransitionType::Script:
				if (mScriptTransitionList[info.ID]->Transition(mData) == true)
				{
					return true;
				}
			case ETransitionType::Bool:
				if (TransitionBool(info) == true)
				{
					return true;
				}
			case ETransitionType::Int:
				if (TransitionInt(info) == true)
				{
					return true;
				}
			case ETransitionType::Float:
				if (TransitionFloat(info) == true)
				{
					return true;
				}
			}
		}
		return false;
	}


	bool AnimationTransition::TransitionBool(const TransitionConditionInfo& info) const
	{
		const TransitionCondition<bool>& condition = mBoolTransitionList[info.ID];
		
		bool lvalue = false;
		bool rvalue = condition.Value;

		if (mData.AnimationParameters->GetBool(condition.Name, &lvalue) == false)
		{
			JG_LOG_ERROR("%s Is Not Exist Animation Paramter", condition.Name);
			return false;
		}
		return lvalue == rvalue;
	}
	bool AnimationTransition::TransitionInt(const TransitionConditionInfo& info) const
	{
		const TransitionCondition<i32>& condition = mIntTransitionList[info.ID];

		i32 lvalue = 0;
		i32 rvalue = condition.Value;

		if (mData.AnimationParameters->GetInt(condition.Name, &lvalue) == false)
		{
			JG_LOG_ERROR("%s Is Not Exist Animation Paramter", condition.Name);
			return false;
		}
		switch (condition.Condition)
		{
			case EAnimationCondition::Equal:
				return lvalue == rvalue;
			case EAnimationCondition::NotEqual:
				return lvalue != rvalue;
			case EAnimationCondition::EqualLess:
				return lvalue <= rvalue;
			case EAnimationCondition::EqualOver:
				return lvalue >= rvalue;
			case EAnimationCondition::Less:
				return lvalue < rvalue;
			case EAnimationCondition::Over:
				return lvalue > rvalue;
			default:
				return false;
		}
		return false;
	}
	bool AnimationTransition::TransitionFloat(const TransitionConditionInfo& info) const
	{

		const TransitionCondition<f32>& condition = mFloatTransitionList[info.ID];

		f32 lvalue = 0.0f;
		f32 rvalue = condition.Value;

		if (mData.AnimationParameters->GetFloat(condition.Name, &lvalue) == false)
		{
			JG_LOG_ERROR("%s Is Not Exist Animation Paramter", condition.Name);
			return false;
		}
		switch (condition.Condition)
		{
		case EAnimationCondition::Equal:
			return lvalue == rvalue;
		case EAnimationCondition::NotEqual:
			return lvalue != rvalue;
		case EAnimationCondition::EqualLess:
			return lvalue <= rvalue;
		case EAnimationCondition::EqualOver:
			return lvalue >= rvalue;
		case EAnimationCondition::Less:
			return lvalue < rvalue;
		case EAnimationCondition::Over:
			return lvalue > rvalue;
		default:
			return false;
		}
		return false;
	}
}