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
	SharedPtr<AnimationClipInfo> AnimationBlendSpace1DInfo::FindAnimationClipInfo(const String& name) const
	{
		if (AnimationClipInfoDic.find(name) == AnimationClipInfoDic.end())
		{
			return nullptr;
		}
		return AnimationClipInfoDic.at(name);
	}

	void AnimationBlendSpace1D::AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, f32 value)
	{
		if (FindAnimationClip(name) != nullptr)
		{
			return;
		}

		mAnimationClipDIc[name]		  = animationClip;
		mAnimClipXParamValueDic[name] = value;
	}

	bool AnimationBlendSpace1D::GetCurrentKeyFrame(const String& nodeName, const AnimationBlendSpace1DInfo& blendInfo, JVector3* T, JQuaternion* Q, JVector3* S)
	{
		f32 currValue = blendInfo.BlendValue;

		blendInfo.BlendSpace1D->GetXParamName();



		String leftNodeName;
		String rightNodeName;

		f32 leftDist = JG_F32_MAX;
		f32 rightDist = JG_F32_MAX;

		ForEach([&](const String& name, SharedPtr<AnimationClip> animClip, f32 value)
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

		SharedPtr<AnimationClip>	 leftClip     = FindAnimationClip(leftNodeName);
		SharedPtr<AnimationClipInfo> leftClipInfo = blendInfo.FindAnimationClipInfo(leftNodeName);

		SharedPtr<AnimationClip>	 rightClip	   = FindAnimationClip(rightNodeName);
		SharedPtr<AnimationClipInfo> rightClipInfo = blendInfo.FindAnimationClipInfo(rightNodeName);
		if (rightClip == nullptr && rightClipInfo == nullptr)
		{
			rightClip    = leftClip;
			rightClipInfo = leftClipInfo;
		}
		f32 factor = leftDist / (rightDist + leftDist);

		JVector3 left_T; JQuaternion left_Q;
		JVector3 left_S;
		if (leftClip->GetCurrentKeyFrame(nodeName, *leftClipInfo, &left_T, &left_Q, &left_S) == false)
		{
			return false;
		}
		JVector3 right_T; JQuaternion right_Q;
		JVector3 right_S;
		if (rightClip->GetCurrentKeyFrame(nodeName, *rightClipInfo, &right_T, &right_Q, &right_S) == false)
		{
			return false;
		}

		if (T) *T = JVector3::Lerp(left_T, right_T, factor);
		if (Q) *Q = JQuaternion::Slerp(left_Q, right_Q, factor);
		if (S) *S = JVector3::Lerp(left_S, right_S, factor);

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
		for (const AnimationBlendSpace1DStock::AnimClipData& clipData : stock.AnimClipDatas)
		{
			SharedPtr<Asset<AnimationClip>> asset = AssetDataBase::GetInstance().LoadOriginAssetImmediate<AnimationClip>(clipData.AssetPath);
			if (asset == nullptr || asset->IsValid() == false)
			{
				continue;
			}

			AddAnimationClip(clipData.Name, asset->Get(), clipData.Value);
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