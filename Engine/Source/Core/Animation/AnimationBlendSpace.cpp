#include "pch.h"
#include "AnimationBlendSpace.h"




namespace JG
{
	// 구간을 나누고 
	// 왼쪽 <-> 오른쪽 Clip 구간
	// 위쪽 <-> 아래쪽 Clpi 구간 구하고 
	// 위쪽 <-> 아래쪽 factor 로lefttop, leftbottom lerp ,  righttop, rightbottom lerp 진행 이후
	// 왼쪽 <-> 오른쪽 factor로 위 lerp한 거 다시 lerp

	AnimationBlendSpaceInfo::AnimationBlendSpaceInfo(const String& name, SharedPtr<AnimationBlendSpace> blendSpace, EAnimationBlendSpaceFlag flags)
	{
	}

	void AnimationBlendSpaceInfo::Reset()
	{
	}

	f32 AnimationBlendSpaceInfo::GetNormalizedTimePos() const
	{
		return f32();
	}

	SharedPtr<AnimationClipInfo> AnimationBlendSpaceInfo::FindAnimationClipInfo(const String& name) const
	{
		return SharedPtr<AnimationClipInfo>();
	}

	void AnimationBlendSpace::AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, f32 value)
	{
	}

	bool AnimationBlendSpace::GetCurrentKeyFrame(const String& nodeName, const AnimationBlendSpaceInfo& blendInfo, JVector3* T, JQuaternion* Q, JVector3* S)
	{
		// Gird 4 4
		JVector2 currValue = blendInfo.GetBlendValue();
		JVector2Int numGrid = GetValueNumGrid();
		JVector2Int gridPos;


		f32 maxXValue = GetMaxXValue();
		f32 maxYValue = GetMaxYValue();

		currValue = JVector2::Max(currValue, JVector2(GetMinXValue(), GetMinYValue()));
		currValue = JVector2::Min(currValue, JVector2(GetMaxXValue(), GetMaxYValue()));

		JVector2 oneUnit = JVector2(GetMaxXValue() / numGrid.x, GetMaxYValue() / numGrid.y);

		gridPos = JVector2Int(currValue.x / oneUnit.x, currValue.y / oneUnit.y);

		// 왼쪽 리스트
		// 오른쪽 리스트
		String lt, lb, rt, rb;

		f32 lt_dist = JG_F32_MAX;
		f32 lb_dist = JG_F32_MAX;
		f32 rt_dist = JG_F32_MAX;
		f32 rb_dist = JG_F32_MAX;

		ForEach([&](const String& name, SharedPtr<AnimationClip> clip, JVector2 value)
		{
			f32 dist = JVector2::Length(currValue - value);

			// 왼쪽
			if (currValue.x <= value.x)
			{
				// 위
				if (currValue.y <= value.y && lt_dist > dist)
				{
					lt_dist = dist;
					lt = name;
				}
				// 아래
				if (currValue.y >= value.y && lb_dist > dist)
				{
					lb_dist = dist;
					lb = name;
				}
			}

			if (currValue.x >= value.x)
			{
				// 위
				if (currValue.y <= value.y && rt_dist > dist)
				{
					rt_dist = dist;
					rt = name;
				}
				// 아래
				if (currValue.y >= value.y && rb_dist > dist)
				{
					rb_dist = dist;
					rb = name;
				}
			}
		});





		return false;
	}


	bool AnimationBlendSpace::IsValid() const
	{
		return false;
	}

	void AnimationBlendSpace::SetAnimationBlendSpaceStock(const AnimationBlendSpaceStock& stock)
	{
	}

	void AnimationBlendSpace::SetName(const String& name)
	{
		mName = name;
	}

	const String& AnimationBlendSpace::GetName() const
	{
		return mName;
	}

	SharedPtr<AnimationClip> AnimationBlendSpace::FindAnimationClip(const String& name) const
	{
		return SharedPtr<AnimationClip>();
	}

	JVector2 AnimationBlendSpace::FindAnimationParamValue(const String& name) const
	{
		return JVector2();
	}

	void AnimationBlendSpace::ForEach(const std::function<void(const String&, SharedPtr<AnimationClip>, JVector2)>& action)
	{
	}

	SharedPtr<AnimationBlendSpace> AnimationBlendSpace::Create(const String& name)
	{
		return SharedPtr<AnimationBlendSpace>();
	}

	SharedPtr<AnimationBlendSpace> AnimationBlendSpace::Create(const AnimationBlendSpaceStock& stock)
	{
		return SharedPtr<AnimationBlendSpace>();
	}

}