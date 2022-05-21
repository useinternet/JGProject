#include "pch.h"
#include "AnimationBlendSpace1D.h"
#include "AnimationClip.h"
#include "AnimationTransform.h"
#include "Application.h"
#include "Class/Asset/Asset.h"
#include "Class/Data/Skeletone.h"
#include "Graphics/Mesh.h"

namespace JG
{
	AnimationBlendSpace1DInfo::AnimationBlendSpace1DInfo(const String& name, SharedPtr<AnimationBlendSpace1D> blendSpace, EAnimationBlendSpace1DFlag flags)
	{
		Name = name;
		BlendSpace1D = blendSpace;

	
		BlendSpace1D->ForEach([&](const String& name, SharedPtr<AnimationClip> animClip,  f32 value)
		{
			AnimationClipInfoDic[name] = CreateSharedPtr<AnimationClipInfo>(name, animClip, EAnimationClipFlags::Repeat);
			AnimationClipInfoDic[name]->SetSpeed(BlendSpace1D->GetAnimationClipSpeed(name));
		});

		bool isDurationDiff = false;
		for (auto _pair : AnimationClipInfoDic)
		{
			if (Duration == 0.0f) Duration = _pair.second->GetDuration();
			else if (Duration != _pair.second->GetDuration())
			{
				isDurationDiff = true;
			}
		}

		if (isDurationDiff == true)
		{
			for (auto _pair : AnimationClipInfoDic)
			{
				_pair.second->SetFlags(_pair.second->GetFlags() | EAnimationClipFlags::Repeat);
			}
			Duration = 0.0f;
		}
	}
	void AnimationBlendSpace1DInfo::Reset()
	{
		TimePos = std::fmodf(TimePos, Duration);
		for (auto _pair : AnimationClipInfoDic)
		{
			_pair.second->Reset();
		}
	}

	f32 AnimationBlendSpace1DInfo::GetNormalizedTimePos() const
	{
		if (Duration == 0.0f) return 1.0f;
		else return TimePos / Duration;
	}
	f32  AnimationBlendSpace1DInfo::GetBlendFactor() const
	{
		return BlendFactor;
	}
	const AnimationClipInfo& AnimationBlendSpace1DInfo::GetLeftBlendingAnimationClipInfo() const
	{
		return *LeftAnimationClipInfo;
	}
	const AnimationClipInfo& AnimationBlendSpace1DInfo::GetRightBlendingAnimationClipInfo() const
	{
		return *RightAnimationClipInfo;
	}
	SharedPtr<AnimationClipInfo> AnimationBlendSpace1DInfo::FindAnimationClipInfo(const String& name) const
	{
		if (AnimationClipInfoDic.find(name) == AnimationClipInfoDic.end())
		{
			return nullptr;
		}
		return AnimationClipInfoDic.at(name);
	}

	void AnimationBlendSpace1DInfo::Update(f32 tick, f32 currValue)
	{
		String leftNodeName;
		String rightNodeName;

		f32 leftDist = JG_F32_MAX;
		f32 rightDist = JG_F32_MAX;

		BlendSpace1D->ForEach([&](const String& name, SharedPtr<AnimationClip> animClip, f32 value)
		{
			f32 dist = Math::Abs(currValue - value);
			if (currValue >= value && dist < leftDist)
			{
				leftDist = dist;
				leftNodeName = name;
			}
			if (currValue < value && dist < rightDist)
			{
				rightDist = dist;
				rightNodeName = name;
			}
		});

		SharedPtr<AnimationClip>	 leftAnimClip = BlendSpace1D->FindAnimationClip(leftNodeName);
		SharedPtr<AnimationClipInfo> leftAnimClipInfo = FindAnimationClipInfo(leftNodeName);

		SharedPtr<AnimationClip>	 rightAnimClip = BlendSpace1D->FindAnimationClip(rightNodeName);
		SharedPtr<AnimationClipInfo> rightAnimClipInfo = FindAnimationClipInfo(rightNodeName);
		if (rightAnimClip == nullptr && rightAnimClipInfo == nullptr)
		{
			rightAnimClip = leftAnimClip;
			rightAnimClipInfo = leftAnimClipInfo;
		}
		if (leftAnimClip == nullptr && leftAnimClipInfo == nullptr)
		{
			leftAnimClip = rightAnimClip;
			leftAnimClipInfo = rightAnimClipInfo;
		}

		if (leftAnimClip != LeftAnimationClip)
		{
			SharedPtr<AnimationClipInfo> prevAnimClipInfo = LeftAnimationClipInfo;

			LeftAnimationClip     = leftAnimClip;
			LeftAnimationClipInfo = leftAnimClipInfo;

			if (prevAnimClipInfo != nullptr)
			{
				LeftAnimationClipInfo->SetNormalizedTimePos(prevAnimClipInfo->GetNormalizedTimePos());
			}
		}
		if (rightAnimClip != RightAnimationClip)
		{
			SharedPtr<AnimationClipInfo> prevAnimClipInfo = RightAnimationClipInfo;

			RightAnimationClip = rightAnimClip;
			RightAnimationClipInfo = rightAnimClipInfo;
			if (prevAnimClipInfo != nullptr)
			{
				RightAnimationClipInfo->SetNormalizedTimePos(prevAnimClipInfo->GetNormalizedTimePos());
			}
		}
		BlendFactor = leftDist / (rightDist + leftDist);

		f32 leftDuration  = LeftAnimationClipInfo->GetDuration() / LeftAnimationClipInfo->GetSpeed();
		f32 rightDuration = RightAnimationClipInfo->GetDuration() / RightAnimationClipInfo->GetSpeed();

		if (LeftAnimationClip != nullptr && RightAnimationClip != nullptr)
		{
			f32 a = 1.0f;
			f32 b = leftDuration / rightDuration;
			f32 leftTimePosFactor = Math::Lerp(a, b, BlendFactor);


			a = rightDuration / leftDuration;
			b = 1.0f;
			f32 rightTimePosFactor = Math::Lerp(a, b, BlendFactor);
		


			LeftAnimationClipInfo->Update(tick, leftTimePosFactor);
			if (LeftAnimationClipInfo != RightAnimationClipInfo)
			{
				RightAnimationClipInfo->Update(tick, rightTimePosFactor);
			}
			
		}
		if (Duration > 0.0f)
		{
			TimePos += tick;
			if (TimePos >= Duration)
			{
				Reset();
			}
		}
	}

	void AnimationBlendSpace1D::AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, f32 value, f32 speed)
	{
		if (FindAnimationClip(name) != nullptr)
		{
			return;
		}

		mAnimationClipDIc[name]		  = animationClip;
		mAnimClipXParamValueDic[name] = value;
		mAnimClipSpeedDic[name]		  = speed;
	}

	bool AnimationBlendSpace1D::GetCurrentKeyFrame(const String& nodeName, const AnimationBlendSpace1DInfo& blendInfo, JVector3* T, JQuaternion* Q, JVector3* S)
	{
		blendInfo.BlendSpace1D->GetXParamName();

		JVector3 left_T; JQuaternion left_Q;
		JVector3 left_S;
		if (blendInfo.LeftAnimationClip->GetCurrentKeyFrame(nodeName, *blendInfo.LeftAnimationClipInfo, &left_T, &left_Q, &left_S) == false)
		{
			return false;
		}
		JVector3 right_T; JQuaternion right_Q;
		JVector3 right_S;
		if (blendInfo.RightAnimationClip->GetCurrentKeyFrame(nodeName, *blendInfo.RightAnimationClipInfo, &right_T, &right_Q, &right_S) == false)
		{
			return false;
		}

		if (T) *T = JVector3::Lerp(left_T, right_T, blendInfo.BlendFactor);
		if (Q) *Q = JQuaternion::Slerp(left_Q, right_Q, blendInfo.BlendFactor);
		if (S) *S = JVector3::Lerp(left_S, right_S, blendInfo.BlendFactor);

		return true;
	}

	void AnimationBlendSpace1D::SetXParamName(const String& paramData)
	{
		mXParamName = paramData;
	}

	const String& AnimationBlendSpace1D::GetXParamName() const
	{
		return mXParamName;
	}

	f32 AnimationBlendSpace1D::GetAnimationClipSpeed(const String& name) const 
	{
		if (mAnimationClipDIc.find(name) == mAnimationClipDIc.end())
		{
			return 0.0f;
		}
		return mAnimClipSpeedDic.at(name);
	}

	bool AnimationBlendSpace1D::IsValid() const
	{
		if (mAnimationClipDIc.empty())
		{
			return false;
		}
		for (auto _pair : mAnimationClipDIc)
		{
			if (_pair.second->IsValid() == false)
			{
				return false;
			}
		}
		return true;
	}

	void AnimationBlendSpace1D::SetAnimationBlendSpace1DStock(const AnimationBlendSpace1DStock& stock)
	{
		mAnimationClipDIc.clear();
		mAnimClipXParamValueDic.clear();

		SetName(stock.Name);
		SetXParamName(stock.XParamName);
		SetMinMaxValue(stock.MinMaxValue.x, stock.MinMaxValue.y);
		for (const AnimationBlendSpace1DStock::AnimClipData& clipData : stock.AnimClipDatas)
		{
			SharedPtr<Asset<AnimationClip>> asset = AssetDataBase::GetInstance().LoadOriginAssetImmediate<AnimationClip>(clipData.AssetPath);
			if (asset == nullptr || asset->IsValid() == false)
			{
				continue;
			}

			AddAnimationClip(clipData.Name, asset->Get(), clipData.Value, clipData.Speed);
			
		}

	}

	void AnimationBlendSpace1D::SetName(const String& name)
	{
		mName = name;
	}

	const String& AnimationBlendSpace1D::GetName() const
	{
		return mName;
	}

	SharedPtr<AnimationClip> AnimationBlendSpace1D::FindAnimationClip(const String& name) const
	{
		if (mAnimationClipDIc.find(name) == mAnimationClipDIc.end())
		{
			return nullptr;
		}
		return mAnimationClipDIc.at(name);
	}



	f32 AnimationBlendSpace1D::FindAnimationXParamValue(const String& name) const
	{
		if (mAnimClipXParamValueDic.find(name) == mAnimClipXParamValueDic.end())
		{
			return 0.0f;
		}
		return mAnimClipXParamValueDic.at(name);
	}

	void AnimationBlendSpace1D::ForEach(const std::function<void(const String&, SharedPtr<AnimationClip>,  f32)>& action)
	{
		for (auto _pair : mAnimationClipDIc)
		{
			SharedPtr<AnimationClip> animClip = _pair.second;
			f32 value = FindAnimationXParamValue(_pair.first);
			if (action)
			{
				action(_pair.first, animClip, value);
			}
		}

	}

	SharedPtr<AnimationBlendSpace1D> AnimationBlendSpace1D::Create(const String& name)
	{
		SharedPtr<AnimationBlendSpace1D> result = CreateSharedPtr< AnimationBlendSpace1D>();
		result->SetName(name);
		return result;
	}

	SharedPtr<AnimationBlendSpace1D> AnimationBlendSpace1D::Create(const AnimationBlendSpace1DStock& stock)
	{
		SharedPtr<AnimationBlendSpace1D> result = CreateSharedPtr< AnimationBlendSpace1D>();
		result->SetAnimationBlendSpace1DStock(stock);
		return result;
	}

}