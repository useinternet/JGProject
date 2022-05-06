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
		Vector2,
		Vector3,
		Vector4
	};

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
