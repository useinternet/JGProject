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
	};

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
