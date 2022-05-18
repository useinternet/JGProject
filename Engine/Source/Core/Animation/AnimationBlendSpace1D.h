#pragma once
#include "JGCore.h"
#include "AnimationDefines.h"

namespace JG
{
	// 언리얼에서 특징을 잘보자
	class AnimationTransform;
	class AnimationBlendSpace1DStock;
	class Skeletone;
	class IMesh;
	class ISubMesh;
	class AnimationClip;
	class AnimationClipInfo;
	class AnimationBlendSpace1DInfo
	{
		friend class AnimationBlendSpace1D;
		friend class AnimationStateMachine;
		friend class AnimationController;
	private:
		String Name;
		f32 TimePos = 0.0f;
		f32 Duration = 0.0f;
		EAnimationBlendSpace1DFlag Flags = EAnimationBlendSpace1DFlag::Repeat;
		SharedPtr<AnimationBlendSpace1D> BlendSpace1D;
		List<SharedPtr<AnimationClipInfo>> ClipInfos;
	public:
		void Reset();
		f32 GetNormalizedTimePos() const {
		}
	};


	class AnimationBlendSpace1D
	{
		String mName;

	public:
		void AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip);
		void RemoveAnimationClip(const String& name);




		bool GetCurrentKeyFrame(const String& nodeName, f32 timePos, JVector3* T, JQuaternion* Q, JVector3* S);





		bool IsValid() const;
		void SetAnimationBlendSpace1DStock(const AnimationBlendSpace1DStock& stock);
		void SetName(const String& name);
		const String& GetName() const;
	
	public:
		static SharedPtr<AnimationBlendSpace1D> Create(const String& name);
		static SharedPtr<AnimationBlendSpace1D> Create(const AnimationBlendSpace1DStock& stock);
	};
}