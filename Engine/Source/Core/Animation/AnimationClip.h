#pragma once
#include "JGCore.h"
#include "AnimationDefines.h"

namespace JG
{
	class AnimationTransform;
	class AnimationClipAssetStock;
	class Skeletone;

	class AnimationClipInfo
	{
		friend class AnimationClip;
		friend class AnimationController;
	private:
		String Name;
		f32    AccTime;
		SharedPtr<AnimationTransform> AnimTransform;
		SharedPtr<AnimationClip> Clip;

	};


	class AnimationClip
	{
		// 애니메이션에 필요한 트랜스폼 계산 -> 컨트롤러에서
		// 클립은 컨트롤러가 가지고있는다.
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
		f32 mDuration	   = 0.0f;
		f32 mTickPerSecond = 0.0f;
		Dictionary<String, AnimationNode> mAnimationNodes;
	public:
		EAnimationClipState Update(SharedPtr<AnimationClipInfo> clipInfo, SharedPtr<JG::Skeletone> skeletone, SharedPtr<AnimationTransform> animTransform);

	private:
		void UpdateInternal(u32 nodeID, f32 tick, SharedPtr<JG::Skeletone> skeletone, const JMatrix& parentTransform, SharedPtr<AnimationTransform> animTransform);

		JVector3 CalcLerpLocation(f32 tick, const AnimationNode* node);
		JVector3 CalcLerpScale(f32 tick, const AnimationNode* node);
		JQuaternion CalcLerpRotation(f32 tick, const AnimationNode* node);

		u32 FindRotation(f32 tick, const AnimationNode* node);
		u32 FindLocation(f32 tick, const AnimationNode* node);
		u32 FindScale(f32 tick, const AnimationNode* node);
		const AnimationNode const* FindAnimationNode(const String& nodeName);
	public:
		static SharedPtr<AnimationClip> Create(const AnimationClipAssetStock& stock);
	};
}