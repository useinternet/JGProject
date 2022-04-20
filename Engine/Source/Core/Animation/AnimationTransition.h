#pragma once
#include "JGCore.h"
#include "AnimationDefines.h"


namespace JG
{
	class AnimationParameters;
	class AnimationClipInfo;


	ENUM_FLAG(EAnimationTransitionFlags)
	enum class EAnimationTransitionFlags
	{
		None,
	};


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
		enum class EConditionType
		{
			Script,
			Bool,
			Int,
			Float,
			Vector2,
			Vector3,
			Vector4,
		};


		template<class T>
		struct TransitionCondition
		{
			String Name;
			T   Value;
		};









		// ConditionType
		// Script
		// 

		// Condition 정보 들어순 대로 검사


		List<SharedPtr<IAnimationTransitionScript>> mScriptList;
		
		EAnimationTransitionFlags mFlags;
		
	public:
		void SetFlags(EAnimationTransitionFlags flags);
		void AddTransitionScript(SharedPtr<IAnimationTransitionScript> Script);
		void AddCondition_Bool();
		void AddCondition_Int();
		void AddCondition_Float();
		void AddCondition_Vector2();
		void AddCondition_Vector3();
		void AddCondition_Vector4();
		bool Transition() const;
	};
}