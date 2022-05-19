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
		f32 TimePos  = 0.0f;
		f32 Duration = 0.0f;
		f32 BlendValue = 0.0f;
		EAnimationBlendSpace1DFlag Flags = EAnimationBlendSpace1DFlag::Repeat;
		SharedPtr<AnimationBlendSpace1D> BlendSpace1D;
		Dictionary<String, SharedPtr<AnimationClipInfo>> AnimationClipInfoDic;
	public:
		AnimationBlendSpace1DInfo(const String& name, SharedPtr<AnimationBlendSpace1D> blendSpace, EAnimationBlendSpace1DFlag flags);

		void Reset();
		f32  GetNormalizedTimePos() const;
		SharedPtr<AnimationClipInfo> FindAnimationClipInfo(const String& name) const;
		f32 GetBlendValue() const {
			return BlendValue;
		}
	};


	class AnimationBlendSpace1D
	{
		String mName;
		String mXParamName;
		JVector2 mMinMaxValue;
		Dictionary<String, f32>							 mAnimClipXParamValueDic;
		Dictionary<String, SharedPtr<AnimationClip>>     mAnimationClipDIc;

	public:
		void AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, f32 value);
		bool GetCurrentKeyFrame(const String& nodeName, const AnimationBlendSpace1DInfo& blendInfo, JVector3* T, JQuaternion* Q, JVector3* S);

		
		void SetXParamName(const String& paramData);
		const String& GetXParamName() const;

		void SetMinMaxValue(f32 minValue, f32 maxValue) {
			mMinMaxValue = JVector2(minValue, maxValue);
		}
		f32 GetMinValue() const {
			return mMinMaxValue.x;
		}
		f32 GetMaxValue() const {
			return mMinMaxValue.y;
		}



		bool IsValid() const;
		void SetAnimationBlendSpace1DStock(const AnimationBlendSpace1DStock& stock);


		void SetName(const String& name);
		const String& GetName() const;

		SharedPtr<AnimationClip>	 FindAnimationClip(const String& name) const;

		f32 FindAnimationXParamValue(const String& name) const;
		void ForEach(const std::function<void(const String&,SharedPtr<AnimationClip>, f32)>& action);
	public:
		static SharedPtr<AnimationBlendSpace1D> Create(const String& name);
		static SharedPtr<AnimationBlendSpace1D> Create(const AnimationBlendSpace1DStock& stock);
	};
}