#include "pch.h"
#include "AnimationStateMachine.h"
#include "AnimationController.h"
#include "AnimationTransform.h"
#include "AnimationTransition.h"
#include "AnimationClip.h"
#include "Application.h"
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
		Clear();
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

		Node* prevNode = FindNode(prevName);
		Node* nextNode = FindNode(nextName);

		AnimationTransition* transition = CreateTransition(prevName, nextName);

		prevNode->ConnectedNode.push_back(nextNode);
		prevNode->ConnectedNodeTransitions.push_back(transition);

		if (makeAction != nullptr)
		{
			makeAction(transition);
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
		ExecuteInternal_Thread(mCurrentNode);
		UpdateFlow_Thread();
		UpdateAnimationTransform_Thread();
		return mFinalAnimationTransforms;
	}
	void AnimationStateMachine::Reset_Thread()
	{
		mPrevNode    = nullptr;
		mCurrentNode = nullptr;
		mFlowNodeStack_Thread = Stack<Node*>();
		mFlow_Thread.NodeList.clear();
		f32 mTransitionTimePos = 0.0f;
		mIsTransitioning = false;
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
	AnimationTransition* AnimationStateMachine::CreateTransition(const String& prevName, const String& nextName)
	{
		AnimationTransitionData transitionData;

		if (GetOwnerAnimationController() != nullptr)
		{
			transitionData.AnimationController = GetOwnerAnimationController();
			transitionData.AnimationParameters = GetOwnerAnimationController()->GetAnimationParameters_Thread().get();
		}
		transitionData.PrevNodeName = prevName;
		transitionData.NextNodeName = nextName;

		String key = prevName + "@#@" + nextName;
		mTransitionDic[key] = CreateSharedPtr<AnimationTransition>(transitionData);

		return mTransitionDic[key].get();
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
	AnimationTransition* AnimationStateMachine::FindTransition(const String& prevName, const String& nextName)
	{
		String key = prevName + "@#@" + nextName;
		if (mTransitionDic.find(key) == mTransitionDic.end())
		{
			return nullptr;
		}
		return mTransitionDic[key].get();
	}
	const String& AnimationStateMachine::FindNodeName(Node* node) const
	{
		
		switch (node->NodeType)
		{
		case ENodeType::AnimationClip:
			return mAnimClipNameList[node->ID];
		default:
			return mBeginNodeName;
		}
		// // O: 여기에 return 문을 삽입합니다.
	}
	void AnimationStateMachine::UpdateFlow_Thread()
	{
		mFlow_Thread.NodeList.clear();

		Node* rootNode = FindNode(mBeginNodeName);
		Stack<Node*> tempStack = mFlowNodeStack_Thread;
		while (tempStack.empty() == false)
		{
			mFlow_Thread.NodeList.push_back(FindNodeName(tempStack.top()));
			tempStack.pop();
		}
		mFlow_Thread.NodeList.push_back(FindNodeName(rootNode));
		std::reverse(mFlow_Thread.NodeList.begin(), mFlow_Thread.NodeList.end());
	}

	void AnimationStateMachine::UpdateFlow_Thread(Node* node)
	{
		Stack<Node*> tempStack = mFlowNodeStack_Thread;
		while (tempStack.empty() == false)
		{
			Node* flowNode = tempStack.top();
			if (flowNode == node)
			{
				break;
			}
			tempStack.pop();
		}
		if (tempStack.empty() == false)
		{
			mFlowNodeStack_Thread = tempStack;
		}
		else
		{
			mFlowNodeStack_Thread.push(node);
		}
	}

	void AnimationStateMachine::UpdateAnimationTransform_Thread()
	{
		AnimationController* animController = GetOwnerAnimationController();
		if (animController == nullptr)
		{
			return;
		}

		switch (mCurrentNode->NodeType)
		{
		case ENodeType::AnimationClip:
			UpdateAnimationTransform_AnimClip_Thread(mCurrentNode, mFinalAnimationTransforms);
			break;
		}
		// 만약 전 트랜스폼
		List<SharedPtr<AnimationTransform>> prevAnimTransforms;
		f32 lerpFactor = 1.0f;
		if (mIsTransitioning == true && mPrevNode != nullptr && mPrevNode->NodeType != ENodeType::Begin)
		{
			mTransitionTimePos += Application::GetInstance().GetAppTimer()->GetTick();
			AnimationTransition* transition = FindTransition(FindNodeName(mPrevNode), FindNodeName(mCurrentNode));
			
			if (transition != nullptr)
			{
				SharedPtr<AnimationClipInfo> currentClipInfo = animController->FindAnimationClipInfo(FindNodeName(mCurrentNode));

				if (mTransitionTimePos > transition->GetTransitionDuration())
				{
					mIsTransitioning = false;
					mTransitionTimePos = 0.0f;
				}
				else
				{
					UpdateAnimationTransform_AnimClip_Thread(mPrevNode, prevAnimTransforms);
				}
				lerpFactor = Math::Min<f32>(mTransitionTimePos / transition->GetTransitionDuration(), 1.0f);
			}
		}

		// Lerp
		if (prevAnimTransforms.empty() == false)
		{
			u32 animTransformCnt = prevAnimTransforms.size();
			for (i32 i = 0; i < animTransformCnt; ++i)
			{
				SharedPtr<AnimationTransform> prev_at = prevAnimTransforms[i];
				SharedPtr<AnimationTransform> next_at = mFinalAnimationTransforms[i];
				for (i32 j = 0; j < prev_at->GetMatrixCount(); ++j)
				{
					JVector3 prev_location; JVector3 prev_scale;
					JQuaternion prev_quat;

					JVector3 next_location; JVector3 next_scale;
					JQuaternion next_quat;

					const JMatrix& prev_m = JMatrix::Transpose(prev_at->Get(j));
					const JMatrix& next_m = JMatrix::Transpose(next_at->Get(j));

					if (prev_m.Decompose(&prev_location, &prev_quat, &prev_scale) == false)
					{
						JG_LOG_ERROR("Fail Decompose While Transitioning");
						break;
					}
					if (next_m.Decompose(&next_location, &next_quat, &next_scale) == false)
					{
						JG_LOG_ERROR("Fail Decompose While Transitioning");
						break;
					}

					JVector3 location = JVector3::Lerp(prev_location, next_location, lerpFactor);
					JQuaternion quat = JQuaternion::Slerp(prev_quat, next_quat, lerpFactor);
					JVector3 scale = JVector3::Lerp(prev_scale, next_scale, lerpFactor);

					mFinalAnimationTransforms[i]->Set(j, 
						JMatrix::Transpose(JMatrix::AffineTransformation(location, quat, scale)));
				}
			}
		}
	}

	void AnimationStateMachine::UpdateAnimationTransform_AnimClip_Thread(Node* node, List<SharedPtr<AnimationTransform>>& out_animTransform)
	{
		AnimationController* animController = GetOwnerAnimationController();

		String name = mAnimClipNameList[node->ID];
		SharedPtr<AnimationClip>	 animClip     = animController->FindAnimationClip(name);
		SharedPtr<AnimationClipInfo> animClipInfo = animController->FindAnimationClipInfo(name);

		if (animClip == nullptr || animClipInfo == nullptr)
		{
			JG_LOG_ERROR("%s is not exist Animation Clip", name);
			return;
		}
		EAnimationClipState clipState = animClip->Update(
			animClipInfo, animController->GetBindedOriginMesh(), animController->GetBindedSkeletone(), out_animTransform);


		switch (clipState)
		{
		case EAnimationClipState::Running:
			break;
		case EAnimationClipState::Compelete:
			if (animClipInfo->GetFlags() & EAnimationClipFlags::Repeat)
			{
				animClipInfo->Reset();
				animClip->Update(animClipInfo, animController->GetBindedOriginMesh(), animController->GetBindedSkeletone(), out_animTransform);
			}
		}
	}

	void AnimationStateMachine::ExecuteInternal_Thread(Node* node)
	{
		if (mCurrentNode != node)
		{
			mIsTransitioning = true;
			mTransitionTimePos = 0.0f;
			mPrevNode    = mCurrentNode;
			mCurrentNode = node;
		}
		u32 transitionCnt = node->ConnectedNode.size();
		for (u32 i = 0; i < transitionCnt; ++i)
		{
			Node* connectedNode = node->ConnectedNode[i];
			AnimationTransition* transiton = node->ConnectedNodeTransitions[i];
			if (transiton->Transition() == true)
			{
				UpdateFlow_Thread(connectedNode);
				ExecuteInternal_Thread(connectedNode);
			}
		}
		
	}
	void AnimationStateMachine::Clear()
	{
		Reset_Thread();
		mNodeDic.clear();
		mAnimClipNameList.clear();
		mBeginNodeName.clear();
	}
}