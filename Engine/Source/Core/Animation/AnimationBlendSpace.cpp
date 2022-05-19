#include "pch.h"
#include "AnimationBlendSpace.h"




namespace JG
{
	// ������ ������ 
	// ���� <-> ������ Clip ����
	// ���� <-> �Ʒ��� Clpi ���� ���ϰ� 
	// ���� <-> �Ʒ��� factor ��lefttop, leftbottom lerp ,  righttop, rightbottom lerp ���� ����
	// ���� <-> ������ factor�� �� lerp�� �� �ٽ� lerp

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

		// ���� ����Ʈ
		// ������ ����Ʈ
		String lt, lb, rt, rb;

		f32 lt_dist = JG_F32_MAX;
		f32 lb_dist = JG_F32_MAX;
		f32 rt_dist = JG_F32_MAX;
		f32 rb_dist = JG_F32_MAX;

		ForEach([&](const String& name, SharedPtr<AnimationClip> clip, JVector2 value)
		{
			f32 dist = JVector2::Length(currValue - value);

			// ����
			if (currValue.x <= value.x)
			{
				// ��
				if (currValue.y <= value.y && lt_dist > dist)
				{
					lt_dist = dist;
					lt = name;
				}
				// �Ʒ�
				if (currValue.y >= value.y && lb_dist > dist)
				{
					lb_dist = dist;
					lb = name;
				}
			}

			if (currValue.x >= value.x)
			{
				// ��
				if (currValue.y <= value.y && rt_dist > dist)
				{
					rt_dist = dist;
					rt = name;
				}
				// �Ʒ�
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