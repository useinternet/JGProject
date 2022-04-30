#include "pch.h"
#include "AnimationSequence.h"
#include "AnimationController.h"
#include "AnimationTransform.h"
#include "AnimationTransition.h"
#include "AnimationClip.h"
namespace JG
{
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

	AnimationSequence& AnimationSequence::Begin(const String& startNodeName)
	{
		Reset();
		mIsMakingSequence = true;
		if (CreateNode(ENodeType::Begin, startNodeName) == false)
		{
			return *this;
		}
		mBeginNodeName = startNodeName;



		return *this;
	}

	AnimationSequence& AnimationSequence::MakeSequenceNode(const String& name, const MakeAnimationSequenceAction& makeAction)
	{
		if (CreateNode(ENodeType::AnimationSequence, name) == false)
		{
			return *this;
		}

		const Node* node = FindNode(name);
		AnimationSequence* pAnimSequence = mAnimSequenceList[node->ID].get();
		if (makeAction != nullptr)
		{
			makeAction(pAnimSequence);
		}

		return *this;
	}

	AnimationSequence& AnimationSequence::MakeAnimationClipNode(const String& name, const MakeAnimationClipAction& makeAction)
	{
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

	AnimationSequence& AnimationSequence::ConnectNode(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction)
	{
		if (IsExistNode(prevName) == false || IsExistNode(nextName) == false)
		{
			return *this;
		}

		AnimationTransitionData transitionData;

		if (GetOwnerAnimationController() != nullptr)
		{
			transitionData.AnimationController = GetOwnerAnimationController();
			transitionData.AnimationParameters = GetOwnerAnimationController()->GetAnimationParameters();
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


	void AnimationSequence::End()
	{
		mIsMakingSequence = false;
	}

	SharedPtr<AnimationTransform> AnimationSequence::Execute()
	{
		if (mCurrentNode == nullptr)
		{
			mCurrentNode = FindNode(mBeginNodeName);
		}
		if(mCurrentNode == nullptr)
		{
			return nullptr;
		}
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
	SharedPtr<AnimationTransform> AnimationSequence::ExecuteInternal(Node* node)
	{
		SharedPtr<AnimationController> animController = GetOwnerAnimationController();
		if (animController == nullptr)
		{
			return nullptr;
		}
		SharedPtr<AnimationTransform> result = nullptr;
		u32 transitionCnt = node->ConnectedNode.size();
		for (u32 i = 0; i < transitionCnt; ++i)
		{
			Node* connectedNode = node->ConnectedNode[i];
			SharedPtr<AnimationTransition> transiton = node->ConnectedNodeTransitions[i];
			if (transiton->Transition() == true)
			{
				result = ExecuteInternal(connectedNode);
			}
		}

		if (result == nullptr)
		{
			switch (node->NodeType)
			{
			case ENodeType::AnimationSequence:
			{
				AnimationSequence* sequence = mAnimSequenceList[node->ID].get();
				result = sequence->Execute();
			}
			break;
			case ENodeType::AnimationClip:
				String name = mAnimClipNameList[node->ID];
				SharedPtr<AnimationClip>	 animClip = animController->FindAnimationClip(name);
				SharedPtr<AnimationClipInfo> animClipInfo = animController->FindAnimationClipInfo(name);

				if (animClip == nullptr || animClipInfo == nullptr)
				{
					JG_LOG_ERROR("%s is not exist Animation Clip", name);
					return nullptr;
				}
				EAnimationClipState clipState = animClip->Update(animClipInfo, animController->GetBindedSkeletone());
				switch (clipState)
				{
				case EAnimationClipState::Running:
				case EAnimationClipState::Compelete:
					if (animClipInfo->GetFlags() & EAnimationClipFlags::Repeat)
					{
						animClipInfo->Reset();
						animClip->Update(animClipInfo, animController->GetBindedSkeletone());
					}
					result = animClipInfo->AnimTransform;
					break;
				default:
					result = nullptr;
				break;
				}

				// clipState 에 따른 대처 고민 해보자
				// Clip이면 애니메이션이 모두 완료될때까지 Clip에서 실행
				// Clip이 완료되면 Transition 검사 이후 
				// 트랜지션이  false 애니메이션 다시 시작 머물러서

				// clipState 에 따른 대처 고민 해보자
				// Clip이면 애니메이션이 모두 완료될때까지 Clip에서 실행
				// Clip이 완료되면 Transition 검사 이후 
				// 트랜지션이  false 애니메이션 다시 시작 머물러서
				// 트랜지션 true면 다음 시퀸스나 클립 노드로 이동
				break;
			}
		}
		
		

		return result;
	}
	void AnimationSequence::Reset()
	{
		mNodeDic.clear();
		mAnimSequenceList.clear();
		mAnimClipNameList.clear();
		mBeginNodeName.clear();
		mCurrentNode = nullptr;
	}
}