#pragma once
#include "JGCore.h"
#include "AnimationDefines.h"

namespace JG
{
	class AnimationTransform;
	class AnimationClipAssetStock;
	class Skeletone;
	class IMesh;
	class ISubMesh;
	class AnimationClip;
	class AnimationClipInfo
	{
		friend class AnimationClip;
		friend class AnimationStateMachine;
		friend class AnimationController;
	private:
		String Name;
		f32    TimePos;
		f32    Duration;
		f32    Speed = 1;
		f32    TickPerSecond = 1.0f;
		SharedPtr<AnimationClip> Clip;
		EAnimationClipFlags Flags = EAnimationClipFlags::None;
	public:
		AnimationClipInfo(const String& name, SharedPtr<AnimationClip> clip, EAnimationClipFlags flags);
		void Reset()
		{
			TimePos = std::fmodf(TimePos, Duration);
		}
		f32 GetNormalizedTimePos() const {
			return TimePos / Duration;
		}
		f32 GetSpeed() const {
			return Speed;
		}
		f32 GetDuration() const {
			return Duration / 10 / TickPerSecond;
		}
		EAnimationClipFlags GetFlags() const
		{
			return Flags;
		}
	};


	class AnimationClip
	{
		struct AnimationNode
		{
			String NodeName;

			List<JVector3>	  LocationValues;
			List<JQuaternion> RotationValues;
			List<JVector3>    ScaleValues;

			List<f32> LocationTimes;
			List<f32> RotationTimes;
			List<f32> ScaleTimes;
		};
	private:
		String mName;
		String mClipName;
		f32 mDuration	   = 0.0f;
		f32 mTickPerSecond = 0.0f;
		Dictionary<String, AnimationNode> mAnimationNodes;
	public:
		bool IsValid() const;
		void SetAnimationClipStock(const AnimationClipAssetStock& stock);
		void SetName(const String& name);
		const String& GetName() const;
		const String& GetClipName() const;
		f32 GetDuration() const;
		f32 GetTickPerSecond() const;

		bool GetCurrentKeyFrame(const String& nodeName, f32 timePos, JVector3* T, JQuaternion* Q, JVector3* S);
	private:
		JVector3 CalcLerpLocation(f32 timePos, const AnimationNode* node);
		JVector3 CalcLerpScale(f32 timePos, const AnimationNode* node);
		JQuaternion CalcLerpRotation(f32 timePos, const AnimationNode* node);

		u32 FindRotation(f32 timePos, const AnimationNode* node);
		u32 FindLocation(f32 timePos, const AnimationNode* node);
		u32 FindScale(f32 timePos, const AnimationNode* node);
		const AnimationNode* FindAnimationNode(const String& nodeName);


		
	public:
		static SharedPtr<AnimationClip> Create(const String& name);
		static SharedPtr<AnimationClip> Create(const AnimationClipAssetStock& stock);
	};
}