#include "pch.h"
#include "AnimationSequence.h"
#include "AnimationController.h"

namespace JG
{
	//AnimationSequenceNexus AnimationSequenceNexus::MakeSequenceNode(const String& name, const MakeAnimationSequenceAction& makeAction)
	//{
	//	if (IsValid() == false)
	//	{
	//		return AnimationSequenceNexus(nullptr);
	//	}
	//	return Pointer->MakeSequenceNode(name, makeAction);
	//}

	//AnimationSequenceNexus AnimationSequenceNexus::MakeAnimationClipNode(const String& clipName, const MakeAnimationClipAction& makeAction)
	//{
	//	if (IsValid() == false)
	//	{
	//		return AnimationSequenceNexus(nullptr);
	//	}
	//	return Pointer->MakeAnimationClipNode(clipName, makeAction);
	//}

	//AnimationSequenceNexus AnimationSequenceNexus::ConnectSequence(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction)
	//{
	//	if (IsValid() == false)
	//	{
	//		return AnimationSequenceNexus(nullptr);
	//	}
	//	return Pointer->ConnectSequence(prevName, nextName, makeAction);
	//}

	//AnimationSequenceNexus AnimationSequenceNexus::ConnectAnimationClip(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction)
	//{
	//	if (IsValid() == false)
	//	{
	//		return AnimationSequenceNexus(nullptr);
	//	}
	//	return Pointer->ConnectAnimationClip(prevName, nextName, makeAction);
	//}

	void AnimationSequenceNexus::End()
	{
		if (IsValid() == false)
		{
			return;
		}
		Pointer->End();
	}

	bool AnimationSequenceNexus::IsValid() const
	{
		return Pointer != nullptr;
	}






	AnimationSequence::AnimationSequence(SharedPtr<AnimationController> controller)
	{
		mOwnerAnimController = controller;
	}

	bool AnimationSequence::IsValid() const
	{
		return mOwnerAnimController.lock() != nullptr;
	}

	SharedPtr<AnimationController> AnimationSequence::GetOwnerAnimationController() const
	{
		return mOwnerAnimController.lock();
	}

	AnimationSequenceNexus AnimationSequence::Begin(const String& startNodeName, const String& endNodeName)
	{
		Reset();
		mIsMakingSequence = true;
		if (CreateNode(ENodeType::Empty, startNodeName) == false)
		{
			return AnimationSequenceNexus(nullptr);
		}
		if (CreateNode(ENodeType::Empty, endNodeName) == false)
		{
			return AnimationSequenceNexus(nullptr);
		}
		mStartNodeName = startNodeName;
		mEndNodeName   = endNodeName;


		return AnimationSequenceNexus(this);
	}

	AnimationSequenceNexus AnimationSequence::MakeSequenceNode(const String& name, const MakeAnimationSequenceAction& makeAction)
	{
		if (CreateNode(ENodeType::AnimationSequence, name) == false)
		{
			return AnimationSequenceNexus(nullptr);
		}

		const Node* node = FindNode(name);
		AnimationSequence* pAnimSequence = mAnimSequenceList[node->ID].get();
		if (makeAction != nullptr)
		{
			makeAction(pAnimSequence);
		}

		return AnimationSequenceNexus(this);
	}

	AnimationSequenceNexus AnimationSequence::MakeAnimationClipNode(const String& name, const MakeAnimationClipAction& makeAction)
	{
		if (CreateNode(ENodeType::AnimationClip, name) == false)
		{
			return AnimationSequenceNexus(nullptr);
		}

		const Node* node = FindNode(name);
		SharedPtr<AnimationClipInfo> pClipInfo = GetOwnerAnimationController()->FindAnimationClipInfo(name);
		if (makeAction != nullptr && pClipInfo != nullptr)
		{
			makeAction(pClipInfo.get());
		}
		return AnimationSequenceNexus(this);
	}

	AnimationSequenceNexus AnimationSequence::ConnectNode(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction)
	{
		if (IsExistNode(prevName) == false || IsExistNode(nextName) == false)
		{
			return AnimationSequenceNexus(nullptr);
		}
		SharedPtr<AnimationTransition> transition = CreateSharedPtr<AnimationTransition>();

		Node* prevNode = FindNode(prevName);
		Node* nextNode = FindNode(nextName);

		prevNode->ConnectedNode.push_back(nextNode);
		prevNode->ConnectedNodeTransitions.push_back(transition);

		if (makeAction != nullptr)
		{
			makeAction(transition.get());
		}
		return AnimationSequenceNexus(this);
	}


	void AnimationSequence::End()
	{
		mIsMakingSequence = false;
	}
	void AnimationSequence::Reset()
	{
		mNodeDic.clear();
		mAnimSequenceList.clear();
		mAnimClipNameList.clear();
		mStartNodeName.clear();
		mEndNodeName.clear();
	}
	SharedPtr<AnimationTransform> AnimationSequence::Execute()
	{

		// 시퀸스 

		// 애니메이션 트랜스폼을 리턴




		return nullptr;
	}
	bool AnimationSequence::CreateNode(ENodeType nodeType, const String& name)
	{
		if (mNodeDic.find(name) != mNodeDic.end())
		{
			return false;
		}

		u32 resultID = -1;
		mNodeDic[name] = CreateUniquePtr<Node>();
		Node& newNode = *(mNodeDic[name].get());
		newNode.ID = -1;
		newNode.NodeType = nodeType;
		switch (nodeType)
		{
		case ENodeType::AnimationSequence:
		{
			newNode.ID = mAnimSequenceList.size();
			mAnimSequenceList.push_back(CreateUniquePtr<AnimationSequence>(GetOwnerAnimationController()));
		}
			break;
		case ENodeType::AnimationClip:
		{
			newNode.ID = mAnimClipNameList.size();
			mAnimClipNameList.push_back(name);
		}
			break;
		}

		return true;
	}
	bool AnimationSequence::IsExistNode(const String& name) const
	{
		return mNodeDic.find(name) != mNodeDic.end();
	}
	AnimationSequence::Node* AnimationSequence::FindNode(const String& name) const
	{
		if (IsExistNode(name) == false)
		{
			return nullptr;
		}
		return mNodeDic.at(name).get();
	}
}