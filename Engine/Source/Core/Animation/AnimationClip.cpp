#include "pch.h"
#include "AnimationClip.h"
#include "AnimationTransform.h"
#include "Application.h"
#include "Class/Asset/Asset.h"
#include "Class/Data/Skeletone.h"


namespace JG
{
	AnimationClipInfo::AnimationClipInfo(const String& name, SharedPtr<AnimationClip> clip, EAnimationClipFlags flags)
	{
		Name = name;
		AccTime = 0.0f;
		AnimTransform = CreateSharedPtr<AnimationTransform>();
		Clip = clip;
		Flags = flags;
	}


	EAnimationClipState AnimationClip::Update(SharedPtr<AnimationClipInfo> clipInfo, SharedPtr<JG::Skeletone> skeletone)
	{
		if (skeletone == nullptr || skeletone->IsValid() == false || clipInfo == nullptr)
		{
			return EAnimationClipState::None;
		}
		f32 tick = Application::GetInstance().GetAppTimer()->GetTick();

		clipInfo->AccTime += tick;
		if (clipInfo->AccTime >= mDuration)
		{
			clipInfo->AccTime = 0.0f;
			return EAnimationClipState::Compelete;
		}

		UpdateInternal(skeletone->GetRootNodeID(), tick, skeletone, JMatrix::Identity(), clipInfo->AnimTransform);

		return EAnimationClipState::Running;
	}
	bool AnimationClip::IsValid() const
	{
		return mDuration > 0.0f;
	}

	void AnimationClip::SetAnimationClipStock(const AnimationClipAssetStock& stock)
	{
		mClipName = stock.Name;
		mDuration = stock.Duration;
		mTickPerSecond = stock.TicksPerSecond;

		for (const auto& _pair : stock.AnimationNodes)
		{
			AnimationNode node;
			node.NodeName = _pair.second.NodeName;
			node.LocationValues = _pair.second.LocationValues;
			node.LocationTimes = _pair.second.LocationTimes;
			node.RotationValues = _pair.second.RotationValues;
			node.RotationTimes = _pair.second.RotationTimes;
			node.ScaleValues = _pair.second.ScaleValues;
			node.ScaleTimes = _pair.second.ScaleTimes;
			mAnimationNodes.emplace(_pair.first, node);
		}
	}

	void AnimationClip::SetName(const String& name)
	{
		mName = name;
	}

	const String& AnimationClip::GetName() const
	{
		return mName;
	}

	const String& AnimationClip::GetClipName() const
	{
		return mClipName;
	}


	void AnimationClip::UpdateInternal(u32 nodeID, f32 tick, SharedPtr<JG::Skeletone> skeletone, const JMatrix& parentTransform, SharedPtr<AnimationTransform> animTransform)
	{
		const Skeletone::Node* node = skeletone->GetNode(nodeID);
		JMatrix nodeTransform = node->Transform;

		const Skeletone::Node* parentNode = skeletone->GetNode(node->ParentNode);

		const AnimationNode* animNode = FindAnimationNode(node->Name);

		if (animNode != nullptr)
		{
			JVector3    Scale = CalcLerpScale(tick, animNode);
			JQuaternion RotationQ = CalcLerpRotation(tick, animNode);
			JVector3    Location = CalcLerpLocation(tick, animNode);

			JMatrix S = JMatrix::Scaling(Scale);
			JMatrix R = JMatrix::Rotation(RotationQ);
			JMatrix T = JMatrix::Translation(Location);


			nodeTransform = S * R * T;
		}


		JMatrix globalTransform = parentTransform * nodeTransform;

		JMatrix finalTransform = skeletone->GetRootOffsetTransform() * globalTransform * node->BoneOffset;
		animTransform->Set(node->ID, finalTransform);


		for (u32 nodeID : node->ChildNodes)
		{
			const Skeletone::Node* childNode = skeletone->GetNode(nodeID);
			UpdateInternal(childNode->ID, tick, skeletone, globalTransform, animTransform);
		}
	}
	JVector3 AnimationClip::CalcLerpLocation(f32 tick, const AnimationNode* node)
	{
		if (node->LocationValues.size() == 1)
		{
			return node->LocationValues[0];
		}
		u32 LocationIndex = FindLocation(tick, node);
		u32 NextLocationIndex = (LocationIndex + 1);


		f32 DeltaTime = node->LocationTimes[NextLocationIndex] - node->LocationTimes[LocationIndex];
		f32 Factor = (tick - node->LocationTimes[LocationIndex]) / DeltaTime;


		JGASSERT(Factor >= 0.0f && Factor <= 1.0f);


		const JVector3& StartLocation = node->LocationValues[LocationIndex];
		const JVector3& EndLocation = node->LocationValues[NextLocationIndex];

		JVector3 Result = JVector3::Lerp(StartLocation, EndLocation, Factor);

		return Result;
	}
	JVector3 AnimationClip::CalcLerpScale(f32 tick, const AnimationNode* node)
	{
		if (node->ScaleValues.size() == 1)
		{
			return node->ScaleValues[0];
		}
		u32 ScaleIndex = FindScale(tick, node);
		u32 NextScaleIndex = (ScaleIndex + 1);


		f32 DeltaTime = node->ScaleTimes[NextScaleIndex] - node->ScaleTimes[ScaleIndex];
		f32 Factor = (tick - node->ScaleTimes[ScaleIndex]) / DeltaTime;


		JGASSERT(Factor >= 0.0f && Factor <= 1.0f);


		const JVector3& StartScale = node->ScaleValues[ScaleIndex];
		const JVector3& EndScale = node->ScaleValues[NextScaleIndex];

		JVector3 Result = JVector3::Lerp(StartScale, EndScale, Factor);

		return Result;
	}
	JQuaternion AnimationClip::CalcLerpRotation(f32 tick, const  AnimationNode* node)
	{

		if (node->RotationValues.size() == 1) {
			return node->RotationValues[0];
		}

		u32 RotationIndex = FindRotation(tick, node);
		u32 NextRotationIndex = (RotationIndex + 1);


		f32 DeltaTime = node->RotationTimes[NextRotationIndex] - node->RotationTimes[RotationIndex];
		f32 Factor = (tick - node->RotationTimes[RotationIndex]) / DeltaTime;
		JGASSERT(Factor >= 0.0f && Factor <= 1.0f);
		const JQuaternion& StartRotationQ = node->RotationValues[RotationIndex];
		const JQuaternion& EndRotationQ    = node->RotationValues[NextRotationIndex];
		
		JQuaternion ResultQ  = JQuaternion::Slerp(StartRotationQ, EndRotationQ, Factor);
		ResultQ = JQuaternion::Normalize(ResultQ);

		return ResultQ;
	}
	u32 AnimationClip::FindRotation(f32 tick, const AnimationNode* node)
	{
		for (u32 i = 0; i < node->RotationTimes.size() - 1; i++) {
			if (tick < node->RotationTimes[i + 1]) {
				return i;
			}
		}
		return 0;
	}
	u32 AnimationClip::FindLocation(f32 tick, const AnimationNode* node)
	{
		for (u32 i = 0; i < node->LocationTimes.size() - 1; i++) {
			if (tick < node->LocationTimes[i + 1]) {
				return i;
			}
		}
		return 0;
	}
	u32 AnimationClip::FindScale(f32 tick, const AnimationNode* node)
	{
		for (u32 i = 0; i < node->ScaleTimes.size() - 1; i++) {
			if (tick < node->ScaleTimes[i + 1]) {
				return i;
			}
		}
		return 0;
	}
	const AnimationClip::AnimationNode* AnimationClip::FindAnimationNode(const String& nodeName)
	{

		if (mAnimationNodes.find(nodeName) == mAnimationNodes.end())
		{
			return nullptr;
		}
		
		return &mAnimationNodes[nodeName];
	}

	

	SharedPtr<AnimationClip> AnimationClip::Create(const String& name)
	{
		return SharedPtr<AnimationClip>();
	}

	SharedPtr<AnimationClip> AnimationClip::Create(const AnimationClipAssetStock& stock)
	{
		SharedPtr<AnimationClip> animClip = CreateSharedPtr<AnimationClip>();
		animClip->mName = stock.Name;
		animClip->mDuration = stock.Duration;
		animClip->mTickPerSecond = stock.TicksPerSecond;
		
		for (const auto& _pair : stock.AnimationNodes)
		{
			AnimationNode node;
			node.NodeName = _pair.second.NodeName;
			node.LocationValues = _pair.second.LocationValues;
			node.LocationTimes  = _pair.second.LocationTimes;
			node.RotationValues = _pair.second.RotationValues;
			node.RotationTimes	= _pair.second.RotationTimes;
			node.ScaleValues	= _pair.second.ScaleValues;
			node.ScaleTimes		= _pair.second.ScaleTimes;
			animClip->mAnimationNodes.emplace(_pair.first, node);
		}
		return animClip;
	}


}