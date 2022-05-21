#pragma once
#include "JGCore.h"
#include "AnimationDefines.h"

/*
Animation Blend Space
Skeletone Viewer
Animation Bone Layer Weight, Mask


// Layer 마다 Transform 계산 
// 후 Weight로 곱해줌
// Mask 지정 -> Mask에 해당하는 본은
*/
namespace JG
{
	// 언리얼에서 특징을 잘보자
	class AnimationBlendSpaceStock;
	class AnimationTransform;
	class Skeletone;
	class IMesh;
	class ISubMesh;
	class AnimationClip;
	class AnimationClipInfo;
	class AnimationBlendSpaceInfo
	{
		friend class AnimationBlendSpace;
		friend class AnimationStateMachine;
		friend class AnimationController;
	private:
		String Name;
		f32 TimePos = 0.0f;
		f32 Duration = 0.0f;
		EAnimationBlendSpaceFlag Flags = EAnimationBlendSpaceFlag::Repeat;
		SharedPtr<AnimationBlendSpace> BlendSpace;
		Dictionary<String, SharedPtr<AnimationClipInfo>> AnimationClipInfoDic;



		SharedPtr<AnimationClipInfo> LeftTopAnimationClipInfo;
		SharedPtr<AnimationClipInfo> LeftBottomAnimationClipInfo;
		SharedPtr<AnimationClipInfo> RightTopAnimationClipInfo;
		SharedPtr<AnimationClipInfo> RightBottomAnimationClipInfo;
		f32 LTBBlendFactor = 0.0f;
		f32 RTBBlendFactor = 0.0f;
		f32 FinalBlendFactor = 0.0f;

	public:
		AnimationBlendSpaceInfo(const String& name, SharedPtr<AnimationBlendSpace> blendSpace, EAnimationBlendSpaceFlag flags);

		void Reset();
		f32  GetNormalizedTimePos() const;
		SharedPtr<AnimationClipInfo> FindAnimationClipInfo(const String& name) const;
	private:
		void Update(f32 tick, const JVector2& blendValue);
	};


	class AnimationBlendSpace
	{
		String mName;
		String mXParamName;
		String mYParamName;
		JVector2 mMinMaxXValue;
		JVector2 mMinMaxYValue;
		JVector2Int mValueXYNumGrid;
		Dictionary<String, JVector2>					mAnimClipParamValueDic;
		Dictionary<String, SharedPtr<AnimationClip>>    mAnimationClipDIc;
		Dictionary<String, f32>					        mAnimClipSpeedDic;
	public:
		void AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, const JVector2& value, f32 speed);
		bool GetCurrentKeyFrame(const String& nodeName, const AnimationBlendSpaceInfo& blendInfo, JVector3* T, JQuaternion* Q, JVector3* S);

		f32 GetAnimationClipSpeed(const String& name) const;
		void SetXParamName(const String& paramName) {
			mXParamName = paramName;
		}
		const String& GetXParamName() const {
			return mXParamName;
		}

		void SetYParamName(const String& paramName) {
			mYParamName = paramName;
		}
		const String& GetYParamName() const {
			return mYParamName;
		}

		void SetValueNumGrid(const JVector2Int& numGrid) {
			mValueXYNumGrid = numGrid;
		}
		const JVector2Int& GetValueNumGrid() const {
			return mValueXYNumGrid;
		}

		void SetMinMaxXValue(f32 minValue, f32 maxValue) {
			mMinMaxXValue = JVector2(minValue, maxValue);
		}
		f32 GetMinXValue() const {
			return mMinMaxXValue.x;
		}
		f32 GetMaxXValue() const {
			return mMinMaxXValue.y;
		}
		void SetMinMaxYValue(f32 minValue, f32 maxValue) {
			mMinMaxYValue = JVector2(minValue, maxValue);
		}
		f32 GetMinYValue() const {
			return mMinMaxYValue.x;
		}
		f32 GetMaxYValue() const {
			return mMinMaxYValue.y;
		}


		bool IsValid() const;
		void SetAnimationBlendSpaceStock(const AnimationBlendSpaceStock& stock);


		void SetName(const String& name);
		const String& GetName() const;

		SharedPtr<AnimationClip>	 FindAnimationClip(const String& name) const;

		JVector2 FindAnimationParamValue(const String& name) const;
		void ForEach(const std::function<void(const String&, SharedPtr<AnimationClip>, JVector2)>& action);
	public:
		static SharedPtr<AnimationBlendSpace> Create(const String& name);
		static SharedPtr<AnimationBlendSpace> Create(const AnimationBlendSpaceStock& stock);
	};
}