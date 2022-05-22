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
		virtual bool Transition(const AnimationTransitionData& transitionData) = 0;
	};

	// 바로 변경
	// 완료하고 변경
	class AnimationTransition
	{
		friend class AnimationTransitionModifier;
		enum class ETransitionType
		{
			Script,
			Bool,
			Int,
			Float,
			Trigger,
		};
		template<class T>
		struct TransitionCondition
		{
			String	Name;
			T		Value;

			EAnimationCondition Condition;

			TransitionCondition(const String& name, const T& value, EAnimationCondition condition) :
				Name(name), Value(value), Condition(condition) {}
		};
		struct TransitionConditionInfo
		{
			u32 ID;
			ETransitionType Type;

			TransitionConditionInfo(u32 id, ETransitionType type) : ID(id), Type(type) {}
		};


		List<SharedPtr<IAnimationTransitionScript>> mScriptTransitionList;
		List<TransitionCondition<bool>> mBoolTransitionList;
		List<TransitionCondition<i32>>  mIntTransitionList;
		List<TransitionCondition<f32>>  mFloatTransitionList;
		List<String> mTriggerTransitionList;


		List<TransitionConditionInfo> mTransitionInfoList;

		AnimationTransitionData   mData;
		EAnimationTransitionFlags mFlags;

		//
		bool mHasExitTime   = false;
		f32  mExitTime = 0.0f;
		f32  mTransitionDuration = 0.0f;
	public:
		AnimationTransition(const AnimationTransitionData& transitionData) : mData(transitionData) {}
		void SetFlags(EAnimationTransitionFlags flags);
		void AddTransitionScript(SharedPtr<IAnimationTransitionScript> Script);

		void AddCondition_Bool(const String& name, bool value);
		void AddCondition_Int(const String& name, i32 value, EAnimationCondition condition);
		void AddCondition_Float(const String& name, f32 value, EAnimationCondition condition);
		void AddCondition_Trigger(const String& name);
		
		bool Transition();

		void SetTransitionDuration(f32 duration) {
			mTransitionDuration = duration;
		}
		f32 GetTransitionDuration() const {
			return mTransitionDuration;
		}
		void SetHasExitTime(bool hasExitTime)
		{
			mHasExitTime = hasExitTime;
		}
		bool HasExitTime() const
		{
			return mHasExitTime;
		}

		void SetExitTime(f32 normalizeTime)
		{
			mExitTime = normalizeTime;
		}
		f32 GetExitTime() const
		{
			return mExitTime;
		}
		
	private:
		bool TransitionBool(const TransitionConditionInfo& info) const;
		bool TransitionInt(const TransitionConditionInfo& info) const;
		bool TransitionFloat(const TransitionConditionInfo& info) const;
		bool TransitionTrigger(const TransitionConditionInfo& info) const;
	};
}