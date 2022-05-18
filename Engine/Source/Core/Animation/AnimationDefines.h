#pragma once
#include "Common/Define.h"

namespace JG
{
	enum class EAnimationParameterType
	{
		Unknown,
		Bool,
		Float,
		Int,
		Count,
	};

	inline String AnimationParameterTypeToString(EAnimationParameterType type)
	{
		switch (type)
		{
		case EAnimationParameterType::Bool: return "Bool";
		case EAnimationParameterType::Float: return "Float";
		case EAnimationParameterType::Int: return "Int";
		default:
			return "Unknown";
		}
	}

	inline EAnimationParameterType StringToAnimationParameterType(const String& name)
	{
		if (name == "Bool")			return EAnimationParameterType::Bool;
		else if (name == "Float")   return EAnimationParameterType::Float;
		else if (name == "Int")     return EAnimationParameterType::Int;
		else return EAnimationParameterType::Unknown;
	}

	enum class EAnimationCondition
	{
		Equal,
		NotEqual,
		EqualOver,
		EqualLess,
		Over,
		Less,
		Count,
	};

	inline String AnimationConditionTypeToString(EAnimationCondition type)
	{
		switch (type)
		{
		case EAnimationCondition::Equal:     return "Equal";
		case EAnimationCondition::NotEqual:  return "NotEqual";
		case EAnimationCondition::EqualOver: return "EqualOver";
		case EAnimationCondition::EqualLess: return "EqualLess";
		case EAnimationCondition::Over:      return "Over";
		case EAnimationCondition::Less:      return "Less";
		default:
			return "Unknown";
		}
	}

	inline EAnimationCondition StringToAnimationConditionType(const String& name)
	{
		if (name == "Equal")			return EAnimationCondition::Equal;
		else if (name == "NotEqual")   return EAnimationCondition::NotEqual;
		else if (name == "EqualOver")     return EAnimationCondition::EqualOver;
		if (name == "EqualLess")			return EAnimationCondition::EqualLess;
		else if (name == "Over")   return EAnimationCondition::Over;
		else if (name == "Less")     return EAnimationCondition::Less;
		else return EAnimationCondition::Equal;
	}


	ENUM_FLAG(EAnimationTransitionFlags)
	enum class EAnimationTransitionFlags
	{
		None,
	};

	ENUM_FLAG(EAnimationClipFlags)
	enum class EAnimationClipFlags
	{
		None, 
		Repeat,
	};



	ENUM_FLAG(EAnimationBlendSpace1DFlag)
	enum class EAnimationBlendSpace1DFlag
	{
		None,
		Repeat,
	};


	enum class EAnimationClipState
	{
		None,
		Running,
		Compelete,
	};

	class AnimationParameters;
	class AnimationController;

	struct AnimationTransitionData
	{
		AnimationParameters* AnimationParameters;
		AnimationController* AnimationController;

		String PrevNodeName;
		String NextNodeName;


		AnimationTransitionData() = default;
	};



}
