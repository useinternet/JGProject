#include "pch.h"
#include "AnimationBlendSpace.h"
#include "AnimationClip.h"

#include "Class/Asset/Asset.h"


namespace JG
{
	AnimationBlendSpaceInfo::AnimationBlendSpaceInfo(const String& name, SharedPtr<AnimationBlendSpace> blendSpace, EAnimationBlendSpaceFlag flags)
	{
		Name = name;
		BlendSpace = blendSpace;


		BlendSpace->ForEach([&](const String& name, SharedPtr<AnimationClip> animClip, JVector2 value)
		{
			AnimationClipInfoDic[name] = CreateSharedPtr<AnimationClipInfo>(name, animClip, EAnimationClipFlags::Repeat);
			AnimationClipInfoDic[name]->SetSpeed(BlendSpace->GetAnimationClipSpeed(name));
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

	void AnimationBlendSpaceInfo::Reset()
	{
		TimePos = std::fmodf(TimePos, Duration);
		for (auto _pair : AnimationClipInfoDic)
		{
			_pair.second->Reset();
		}
	}

	f32 AnimationBlendSpaceInfo::GetNormalizedTimePos() const
	{
		if (Duration == 0.0f) return 1.0f;
		else return TimePos / Duration;
	}

	SharedPtr<AnimationClipInfo> AnimationBlendSpaceInfo::FindAnimationClipInfo(const String& name) const
	{
		if (AnimationClipInfoDic.find(name) == AnimationClipInfoDic.end())
		{
			return nullptr;
		}
		return AnimationClipInfoDic.at(name);
	}

	void AnimationBlendSpaceInfo::Update(f32 tick, const JVector2& blendValue)
	{
		JVector2 currValue = blendValue;
		f32 maxXValue = BlendSpace->GetMaxXValue();
		f32 minXValue = BlendSpace->GetMinXValue();
		f32 maxYValue = BlendSpace->GetMaxYValue();
		f32 minYValue = BlendSpace->GetMinYValue();

		currValue = JVector2::Max(currValue, JVector2(minXValue, minYValue));
		currValue = JVector2::Min(currValue, JVector2(maxXValue, maxYValue));

		String lt, lb, rt, rb;

		f32 lt_dist = JG_F32_MAX;
		f32 lb_dist = JG_F32_MAX;
		f32 rt_dist = JG_F32_MAX;
		f32 rb_dist = JG_F32_MAX;

		BlendSpace->ForEach([&](const String& name, SharedPtr<AnimationClip> clip, JVector2 value)
		{
			f32 dist = JVector2::LengthSqrd(currValue - value);

			// 왼쪽
			if (currValue.x >= value.x)
			{
				// 위
				if (currValue.y >= value.y && lt_dist > dist)
				{
					lt_dist = dist;
					lt = name;
				}
				// 아래
				if (currValue.y <= value.y && lb_dist > dist)
				{
					lb_dist = dist;
					lb = name;
				}
			}

			if (currValue.x <= value.x)
			{
				// 위
				if (currValue.y >= value.y && rt_dist > dist)
				{
					rt_dist = dist;
					rt = name;
				}
				// 아래
				if (currValue.y <= value.y && rb_dist > dist)
				{
					rb_dist = dist;
					rb = name;
				}
			}
		});

		SharedPtr<AnimationClipInfo> lt_ClipInfo = FindAnimationClipInfo(lt);
		SharedPtr<AnimationClipInfo> lb_ClipInfo = FindAnimationClipInfo(lb);
		SharedPtr<AnimationClipInfo> rt_ClipInfo = FindAnimationClipInfo(rt);
		SharedPtr<AnimationClipInfo> rb_ClipInfo = FindAnimationClipInfo(rb);

		if (lt_ClipInfo == nullptr) {
			lt_ClipInfo = lb_ClipInfo;
			lt = lb;
		}
		if (lb_ClipInfo == nullptr) {
			lb_ClipInfo = lt_ClipInfo;
			lb = lt;
		}
		if (rt_ClipInfo == nullptr) {
			rt_ClipInfo = rb_ClipInfo;
			rt = rb;
		}
		if (rb_ClipInfo == nullptr) {
			rb_ClipInfo = rt_ClipInfo;
			rb = rt;
		}

		if (lt_ClipInfo == nullptr && lb_ClipInfo == nullptr)
		{
			lt_ClipInfo = rt_ClipInfo;
			lb_ClipInfo = rb_ClipInfo;
			lt = rt;
			lb = rb;
		}
		if (rb_ClipInfo == nullptr && rt_ClipInfo == nullptr)
		{
			rt_ClipInfo = lt_ClipInfo;
			rb_ClipInfo = lb_ClipInfo;
			rt = lt;
			rb = lb;
		}
		if (lt_ClipInfo != LeftTopAnimationClipInfo)
		{
			SharedPtr<AnimationClipInfo> prevAnimClipInfo = LeftTopAnimationClipInfo;

			LeftTopAnimationClipInfo = lt_ClipInfo;

			if (prevAnimClipInfo != nullptr)
			{
				LeftTopAnimationClipInfo->SetNormalizedTimePos(prevAnimClipInfo->GetNormalizedTimePos());
			}
		}
		if (lb_ClipInfo != LeftBottomAnimationClipInfo)
		{
			SharedPtr<AnimationClipInfo> prevAnimClipInfo = LeftBottomAnimationClipInfo;

			LeftBottomAnimationClipInfo = lb_ClipInfo;

			if (prevAnimClipInfo != nullptr)
			{
				LeftBottomAnimationClipInfo->SetNormalizedTimePos(prevAnimClipInfo->GetNormalizedTimePos());
			}
		}


		if (rt_ClipInfo != RightTopAnimationClipInfo)
		{
			SharedPtr<AnimationClipInfo> prevAnimClipInfo = RightTopAnimationClipInfo;

			RightTopAnimationClipInfo = rt_ClipInfo;

			if (prevAnimClipInfo != nullptr)
			{
				RightTopAnimationClipInfo->SetNormalizedTimePos(prevAnimClipInfo->GetNormalizedTimePos());
			}
		}
		if (rb_ClipInfo != RightBottomAnimationClipInfo)
		{
			SharedPtr<AnimationClipInfo> prevAnimClipInfo = RightBottomAnimationClipInfo;

			RightBottomAnimationClipInfo = rb_ClipInfo;

			if (prevAnimClipInfo != nullptr)
			{
				RightBottomAnimationClipInfo->SetNormalizedTimePos(prevAnimClipInfo->GetNormalizedTimePos());
			}
		}


		JVector2 ltPos = BlendSpace->FindAnimationParamValue(lt);
		JVector2 lbPos = BlendSpace->FindAnimationParamValue(lb);
		JVector2 rtPos = BlendSpace->FindAnimationParamValue(rt);
		JVector2 rbPos = BlendSpace->FindAnimationParamValue(rb);

		LTBBlendFactor = (currValue.y - ltPos.y) / Math::Max(lbPos.y - ltPos.y, 0.000001f);
		LTBBlendFactor = Math::Clamp<f32>(LTBBlendFactor, 0, 1);
		RTBBlendFactor = (currValue.y - rtPos.y) / Math::Max(rbPos.y - rtPos.y, 0.000001f);
		RTBBlendFactor = Math::Clamp<f32>(RTBBlendFactor, 0, 1);

		f32 left_x = Math::Lerp(ltPos.x, lbPos.x, LTBBlendFactor);
		f32 right_x = Math::Lerp(rtPos.x, rbPos.x, RTBBlendFactor);
		FinalBlendFactor = (currValue.x - left_x) / Math::Max(right_x - left_x, 0.000001f);
		FinalBlendFactor = Math::Clamp<f32>(FinalBlendFactor, 0, 1);

		HashSet<SharedPtr<AnimationClipInfo>> cacheUpdateSet;
		if (LeftTopAnimationClipInfo != nullptr && LeftBottomAnimationClipInfo != nullptr &&
			RightTopAnimationClipInfo != nullptr && RightBottomAnimationClipInfo != nullptr)
		{
			f32 ltAnimDuration = LeftTopAnimationClipInfo->GetDuration() / LeftTopAnimationClipInfo->GetSpeed();
			f32 rtAnimDuration = RightTopAnimationClipInfo->GetDuration() / RightTopAnimationClipInfo->GetSpeed();
			f32 lbAnimDuration = LeftBottomAnimationClipInfo->GetDuration() / LeftBottomAnimationClipInfo->GetSpeed();
			f32 rbAnimDuration = RightBottomAnimationClipInfo->GetDuration() / RightBottomAnimationClipInfo->GetSpeed();

			// LT
			f32 a1 = 1.0f;
			f32 b1 = ltAnimDuration / lbAnimDuration;
			f32 factor1 = Math::Lerp(a1, b1, LTBBlendFactor);


			f32 a2 = 1.0f;
			f32 b2 = rtAnimDuration / rbAnimDuration;
			f32 factor2 = Math::Lerp(a2, b2, RTBBlendFactor);

			f32 a3 = 1.0f;
			f32 b3 = ltAnimDuration / rtAnimDuration;
			f32 factor3 = Math::Lerp(a3, b3, FinalBlendFactor);

			f32 factor4 = Math::Lerp(factor1, factor2, FinalBlendFactor);
			LeftTopAnimationClipInfo->Update(tick, factor3 * factor4);
			cacheUpdateSet.insert(LeftTopAnimationClipInfo);

			// RT
			if (cacheUpdateSet.find(RightTopAnimationClipInfo) == cacheUpdateSet.end())
			{
				a3 = rtAnimDuration / ltAnimDuration;
				b3 = 1.0f;
				factor3 = Math::Lerp(a3, b3, FinalBlendFactor);

				factor4 = Math::Lerp(factor2, factor1, FinalBlendFactor);
				RightTopAnimationClipInfo->Update(tick, factor3 * factor4);
				cacheUpdateSet.insert(RightTopAnimationClipInfo);
			}

			// LB
			a1 = lbAnimDuration / ltAnimDuration;
			b1 = 1.0f;
			factor1 = Math::Lerp(a1, b1, LTBBlendFactor);

			a2 = rbAnimDuration / rtAnimDuration;
			b2 = 1.0f;
			factor2 = Math::Lerp(a2, b2, RTBBlendFactor);



			if (cacheUpdateSet.find(LeftBottomAnimationClipInfo) == cacheUpdateSet.end())
			{
				a3 = 1.0f;
				b3 = lbAnimDuration / rbAnimDuration;
				factor3 = Math::Lerp(a3, b3, FinalBlendFactor);

				factor4 = Math::Lerp(factor1, factor2, FinalBlendFactor);
				LeftBottomAnimationClipInfo->Update(tick, factor3 * factor4);
				cacheUpdateSet.insert(LeftBottomAnimationClipInfo);
			}
			// RB
			if (cacheUpdateSet.find(RightBottomAnimationClipInfo) == cacheUpdateSet.end())
			{
				a3 = rbAnimDuration / lbAnimDuration;
				b3 = 1.0f;
				factor3 = Math::Lerp(a3, b3, FinalBlendFactor);

				factor4 = Math::Lerp(factor2, factor1, FinalBlendFactor);
				RightBottomAnimationClipInfo->Update(tick, factor3 * factor4);
				cacheUpdateSet.insert(RightBottomAnimationClipInfo);
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

	void AnimationBlendSpace::AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, const JVector2& value, f32 speed)
	{

		if (FindAnimationClip(name) != nullptr)
		{
			return;
		}

		mAnimationClipDIc[name] = animationClip;
		mAnimClipParamValueDic[name] = value;
		mAnimClipSpeedDic[name] = speed;
	}

	bool AnimationBlendSpace::GetCurrentKeyFrame(const String& nodeName, const AnimationBlendSpaceInfo& blendInfo, JVector3* T, JQuaternion* Q, JVector3* S)
	{


		JVector3 ltb_T, ltb_S;
		JQuaternion ltb_Q;

		{
		
			JVector3 lt_T, lt_S;
			JQuaternion lt_Q;
			if (blendInfo.LeftTopAnimationClipInfo->GetClip()->GetCurrentKeyFrame(nodeName, *blendInfo.LeftTopAnimationClipInfo, &lt_T, &lt_Q, &lt_S) == false)
			{
				return false;
			}


			JVector3 lb_T, lb_S;
			JQuaternion lb_Q;
			if (blendInfo.LeftBottomAnimationClipInfo->GetClip()->GetCurrentKeyFrame(nodeName, *blendInfo.LeftBottomAnimationClipInfo, &lb_T, &lb_Q, &lb_S) == false)
			{
				return false;
			}

			ltb_T = JVector3::Lerp(lt_T, lb_T, blendInfo.LTBBlendFactor);
			ltb_Q = JQuaternion::Slerp(lt_Q, lb_Q, blendInfo.LTBBlendFactor);
			ltb_S = JVector3::Lerp(lt_S, lb_S, blendInfo.LTBBlendFactor);
		}


		JVector3 rtb_T, rtb_S;
		JQuaternion rtb_Q;

		{

			JVector3 rt_T, rt_S;
			JQuaternion rt_Q;
			if (blendInfo.RightTopAnimationClipInfo->GetClip()->GetCurrentKeyFrame(nodeName, *blendInfo.RightTopAnimationClipInfo, &rt_T, &rt_Q, &rt_S) == false)
			{
				return false;
			}


			JVector3 rb_T, rb_S;
			JQuaternion rb_Q;
			if (blendInfo.RightBottomAnimationClipInfo->GetClip()->GetCurrentKeyFrame(nodeName, *blendInfo.RightBottomAnimationClipInfo, &rb_T, &rb_Q, &rb_S) == false)
			{
				return false;
			}

			rtb_T = JVector3::Lerp(rt_T, rb_T, blendInfo.RTBBlendFactor);
			rtb_Q = JQuaternion::Slerp(rt_Q, rb_Q, blendInfo.RTBBlendFactor);
			rtb_S = JVector3::Lerp(rt_S, rb_S, blendInfo.RTBBlendFactor);
		}


		if (T) *T = JVector3::Lerp(ltb_T, rtb_T, blendInfo.FinalBlendFactor);
		if (Q) *Q = JQuaternion::Slerp(ltb_Q, rtb_Q, blendInfo.FinalBlendFactor);
		if (S) *S = JVector3::Lerp(ltb_S, rtb_S, blendInfo.FinalBlendFactor);

		return true;
	}

	f32 AnimationBlendSpace::GetAnimationClipSpeed(const String& name) const
	{
		if (mAnimationClipDIc.find(name) == mAnimationClipDIc.end())
		{
			return 0.0f;
		}

		return mAnimClipSpeedDic.at(name);
	}


	bool AnimationBlendSpace::IsValid() const
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

	void AnimationBlendSpace::SetAnimationBlendSpaceStock(const AnimationBlendSpaceStock& stock)
	{
		mAnimationClipDIc.clear();
		mAnimClipParamValueDic.clear();

		SetName(stock.Name);
		SetXParamName(stock.XParamName);
		SetYParamName(stock.YParamName);
		SetValueNumGrid(stock.ValueXYNumGrid);
		SetMinMaxXValue(stock.MinMaxXValue.x, stock.MinMaxXValue.y);
		SetMinMaxYValue(stock.MinMaxYValue.x, stock.MinMaxYValue.y);
		for (const AnimationBlendSpaceStock::AnimClipData& clipData : stock.AnimClipDatas)
		{
			SharedPtr<Asset<AnimationClip>> asset = AssetDataBase::GetInstance().LoadOriginAssetImmediate<AnimationClip>(clipData.AssetPath);
			if (asset == nullptr || asset->IsValid() == false)
			{
				continue;
			}

			AddAnimationClip(clipData.Name, asset->Get(), clipData.Value, clipData.Speed);
		}

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
		if (mAnimationClipDIc.find(name) == mAnimationClipDIc.end())
		{
			return nullptr;
		}
		return mAnimationClipDIc.at(name);
	}

	JVector2 AnimationBlendSpace::FindAnimationParamValue(const String& name) const
	{
		if (mAnimClipParamValueDic.find(name) == mAnimClipParamValueDic.end())
		{
			return JVector2(0, 0);
		}
		return mAnimClipParamValueDic.at(name);
	}

	void AnimationBlendSpace::ForEach(const std::function<void(const String&, SharedPtr<AnimationClip>, JVector2)>& action)
	{
		for (auto _pair : mAnimationClipDIc)
		{
			String name = _pair.first;
			SharedPtr<AnimationClip> clip = _pair.second;
			JVector2 value = FindAnimationParamValue(name);

			if (action)
			{
				action(name, clip, value);
			}
		}



	}

	SharedPtr<AnimationBlendSpace> AnimationBlendSpace::Create(const String& name)
	{
		SharedPtr<AnimationBlendSpace> animBlendSpace = CreateSharedPtr<AnimationBlendSpace>();
		animBlendSpace->SetName(name);
		return animBlendSpace;
	}

	SharedPtr<AnimationBlendSpace> AnimationBlendSpace::Create(const AnimationBlendSpaceStock& stock)
	{
		SharedPtr<AnimationBlendSpace> animBlendSpace = CreateSharedPtr<AnimationBlendSpace>();
		animBlendSpace->SetAnimationBlendSpaceStock(stock);
		return animBlendSpace;
	}

}