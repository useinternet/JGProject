#include "pch.h"
#include "AnimationStateMachine.h"
#include "AnimationController.h"
#include "AnimationTransform.h"
#include "AnimationTransition.h"
#include "AnimationClip.h"
namespace JG
{
	AnimationStateMachine::AnimationStateMachine(AnimationController* controller)
	{
		mOwnerAnimController = controller;
	}

	bool AnimationStateMachine::IsValid() const
	{
		return mOwnerAnimController != nullptr;
	}

	AnimationController* AnimationStateMachine::GetOwnerAnimationController() const
	{
		return mOwnerAnimController;
	}



	AnimationStateMachine& AnimationStateMachine::Begin(const String& startNodeName)
	{
		if (mIsRunning) *this;
		Reset();
		mIsMakingMachine = true;
		if (CreateNode(ENodeType::Begin, startNodeName) == false)
		{
			return *this;
		}
		mBeginNodeName = startNodeName;
		return *this;
	}


	AnimationStateMachine& AnimationStateMachine::MakeAnimationClipNode(const String& name, const MakeAnimationClipAction& makeAction)
	{
		if (mIsRunning) *this;
		if (mIsMakingMachine == false)
		{
			return *this;
		}
		if (CreateNode(ENodeType::AnimationClip, name) == false)
		{
			return *this;
		}

		const Node* node = FindNode(name);
		SharedPtr<AnimationClipInfo> pClipInfo = GetOwnerAnimationController()->FindAnimationClipInfo(name);
		if (makeAction != nullptr && pClipInfo != nullptr)
		{
			makeAction(pClipInfo.get());
		}
		return *this;
	}

	AnimationStateMachine& AnimationStateMachine::ConnectNode(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction)
	{
		if (mIsRunning) *this;
		if (mIsMakingMachine == false)
		{
			return *this;
		}
		if (IsExistNode(prevName) == false || IsExistNode(nextName) == false)
		{
			return *this;
		}

		AnimationTransitionData transitionData;

		if (GetOwnerAnimationController() != nullptr)
		{
			transitionData.AnimationController = GetOwnerAnimationController();
			transitionData.AnimationParameters = GetOwnerAnimationController()->GetAnimationParameters_Thread().get();
		}
		transitionData.PrevNodeName = prevName;
		transitionData.NextNodeName = nextName;

		SharedPtr<AnimationTransition> transition = CreateSharedPtr<AnimationTransition>(transitionData);

		Node* prevNode = FindNode(prevName);
		Node* nextNode = FindNode(nextName);

		prevNode->ConnectedNode.push_back(nextNode);
		prevNode->ConnectedNodeTransitions.push_back(transition);

		if (makeAction != nullptr)
		{
			makeAction(transition.get());
		}
		return *this;
	}


	void AnimationStateMachine::End()
	{
		if (mIsRunning) return;
		mIsMakingMachine = false;
	}

	const AnimationStateFlow& AnimationStateMachine::GetAnimationStateFlow_Thread() const
	{
		return mFlow_Thread;
	}
	List<SharedPtr<AnimationTransform>> AnimationStateMachine::Execute_Thread()
	{
		if (mLock == true)
		{
			return List<SharedPtr<AnimationTransform>>();
		}
		if (mCurrentNode == nullptr)
		{
			mCurrentNode = FindNode(mBeginNodeName);
		}
		if(mCurrentNode == nullptr)
		{
			return List<SharedPtr<AnimationTransform>>();
		}
		mIsRunning = true;
		mFlow_Thread.NodeList.clear();
		mFlow_Thread.NodeList.push_back(mBeginNodeName);
		return ExecuteInternal_Thread(mCurrentNode);
	}
	bool AnimationStateMachine::CreateNode(ENodeType nodeType, const String& name)
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
		case ENodeType::AnimationClip:
		{
			newNode.ID = mAnimClipNameList.size();
			mAnimClipNameList.push_back(name);
		}
			break;
		}

		return true;
	}
	bool AnimationStateMachine::IsExistNode(const String& name) const
	{
		return mNodeDic.find(name) != mNodeDic.end();
	}
	AnimationStateMachine::Node* AnimationStateMachine::FindNode(const String& name) const
	{
		if (IsExistNode(name) == false)
		{
			return nullptr;
		}
		return mNodeDic.at(name).get();
	}
	List<SharedPtr<AnimationTransform>> AnimationStateMachine::ExecuteInternal_Thread(Node* node)
	{
		AnimationController* animController = GetOwnerAnimationController();
		if (animController == nullptr)
		{
			return List<SharedPtr<AnimationTransform>>();
		}
		List<SharedPtr<AnimationTransform>> result;
		u32 transitionCnt = node->ConnectedNode.size();
		for (u32 i = 0; i < transitionCnt; ++i)
		{
			Node* connectedNode = node->ConnectedNode[i];
			SharedPtr<AnimationTransition> transiton = node->ConnectedNodeTransitions[i];
			if (transiton->Transition() == true)
			{
				mFlow_Thread.NodeList.push_back(mAnimClipNameList[connectedNode->ID]);
				result = ExecuteInternal_Thread(connectedNode);
			}
		}

		if (result.empty())
		{
			switch (node->NodeType)
			{
			case ENodeType::AnimationClip:
				String name = mAnimClipNameList[node->ID];
				SharedPtr<AnimationClip>	 animClip = animController->FindAnimationClip(name);
				SharedPtr<AnimationClipInfo> animClipInfo = animController->FindAnimationClipInfo(name);

				if (animClip == nullptr || animClipInfo == nullptr)
				{
					JG_LOG_ERROR("%s is not exist Animation Clip", name);
					return List<SharedPtr<AnimationTransform>>();
				}
				EAnimationClipState clipState = animClip->Update(
					animClipInfo, animController->GetBindedOriginMesh(), animController->GetBindedSkeletone(), result);
				switch (clipState)
				{
				case EAnimationClipState::Running:
					break;
				case EAnimationClipState::Compelete:
					if (animClipInfo->GetFlags() & EAnimationClipFlags::Repeat)
					{
						animClipInfo->Reset();
						animClip->Update(animClipInfo, animController->GetBindedOriginMesh(), animController->GetBindedSkeletone(), result);
					}
					break;
				default:
					result.clear();
				break;
				}
			}
		}
		
		

		return result;
	}
	void AnimationStateMachine::Reset()
	{
		mNodeDic.clear();

		mAnimClipNameList.clear();
		mBeginNodeName.clear();
		mCurrentNode = nullptr;
	}
}