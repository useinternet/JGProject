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
		friend class AnimationBlendSpace1DInfo;
		friend class AnimationBlendSpaceInfo;
	private:
		String Name;
		f32    TimePos = 0.0f;
		f32    Duration = 0.0f;
		f32    Speed = 1;
		f32    TickPerSecond = 1.0f;
		SharedPtr<AnimationClip> Clip;
		EAnimationClipFlags Flags = EAnimationClipFlags::None;
		EAnimationClipState State = EAnimationClipState::None;
		AnimationClipInfo() = default;
	public:

		AnimationClipInfo(const String& name, SharedPtr<AnimationClip> clip, EAnimationClipFlags flags);
		void Reset()
		{
			TimePos = 0.0f;
		}
		void SetNormalizedTimePos(f32 normalizedTimePos) {
			normalizedTimePos = Math::Clamp(normalizedTimePos, 0.0f, 1.0f);
			TimePos = Duration * normalizedTimePos;
		}
		f32 GetNormalizedTimePos() const {
			return TimePos / Duration;
		}
		f32 GetSpeed() const {
			return Speed;
		}
		void SetSpeed(f32 speed) {
			Speed = speed;
		}
		f32 GetDuration() const {
			return Duration / 10 / TickPerSecond;
		}
		const String& GetName() const {
			return Name;
		}
		SharedPtr<AnimationClip> GetClip() const {
			return Clip;
		}
		EAnimationClipFlags GetFlags() const
		{
			return Flags;
		}
		void SetFlags(EAnimationClipFlags clipFlags)
		{
			Flags = clipFlags;
		}
		EAnimationClipState GetState() const {
			return State;
		}
	private:
		void Update(f32 tick, f32 multiplier = 1.0f);
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

		bool GetCurrentKeyFrame(const String& nodeName, const AnimationClipInfo& clipInfo, JVector3* T, JQuaternion* Q, JVector3* S);
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