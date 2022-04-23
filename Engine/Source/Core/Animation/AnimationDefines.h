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
		SharedPtr<AnimationParameters> AnimationParameters;
		SharedPtr<AnimationController> AnimationController;

		String PrevNodeName;
		String NextNodeName;
		AnimationTransitionData() = default;
	};



}
