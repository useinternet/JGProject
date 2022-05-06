#include "pch.h"
#include "AnimationSequence.h"
#include "AnimationController.h"
#include "AnimationTransform.h"
#include "AnimationTransition.h"
#include "AnimationClip.h"
namespace JG
{
	AnimationSequence::AnimationSequence(AnimationController* controller)
	{
		mOwnerAnimController = controller;
	}

	bool AnimationSequence::IsValid() const
	{
		return mOwnerAnimController != nullptr;
	}

	AnimationController* AnimationSequence::GetOwnerAnimationController() const
	{
		return mOwnerAnimController;
	}

	AnimationSequence& AnimationSequence::Begin(const String& startNodeName)
	{
		if (mIsRunning) *this;
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
		if (mIsRunning) *this;
		if (mIsMakingSequence == false)
		{
			return *this;
		}
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
		if (mIsRunning) *this;
		if (mIsMakingSequence == false)
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

	AnimationSequence& AnimationSequence::ConnectNode(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction)
	{
		if (mIsRunning) *this;
		if (mIsMakingSequence == false)
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
			transitionData.AnimationParameters = GetOwnerAnimationController()->GetAnimationParameters().get();
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
		if (mIsRunning) return;
		mIsMakingSequence = false;
	}

	List<SharedPtr<AnimationTransform>> AnimationSequence::Execute()
	{
		if (mCurrentNode == nullptr)
		{
			mCurrentNode = FindNode(mBeginNodeName);
		}
		if(mCurrentNode == nullptr)
		{
			return List<SharedPtr<AnimationTransform>>();
		}
		mIsRunning = true;
		return ExecuteInternal(mCurrentNode);
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
	List<SharedPtr<AnimationTransform>> AnimationSequence::ExecuteInternal(Node* node)
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
				result = ExecuteInternal(connectedNode);
			}
		}

		if (result.empty())
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
					return List<SharedPtr<AnimationTransform>>();
				}
				EAnimationClipState clipState = animClip->Update(
					animClipInfo, animController->GetBindedOriginMesh(), animController->GetBindedSkeletone(), result);
				switch (clipState)
				{
				case EAnimationClipState::Running:
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

				// clipState �� ���� ��ó ��� �غ���
				// Clip�̸� �ִϸ��̼��� ��� �Ϸ�ɶ����� Clip���� ����
				// Clip�� �Ϸ�Ǹ� Transition �˻� ���� 
				// Ʈ��������  false �ִϸ��̼� �ٽ� ���� �ӹ�����

				// clipState �� ���� ��ó ��� �غ���
				// Clip�̸� �ִϸ��̼��� ��� �Ϸ�ɶ����� Clip���� ����
				// Clip�� �Ϸ�Ǹ� Transition �˻� ���� 
				// Ʈ��������  false �ִϸ��̼� �ٽ� ���� �ӹ�����
				// Ʈ������ true�� ���� �������� Ŭ�� ���� �̵�
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