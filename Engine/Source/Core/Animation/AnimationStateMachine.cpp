#include "pch.h"
#include "AnimationStateMachine.h"
#include "AnimationController.h"
#include "AnimationTransform.h"
#include "AnimationTransition.h"
#include "AnimationClip.h"
#include "Application.h"
#include "Class/Asset/Asset.h"
#include "Class/Data/Skeletone.h"
#include "Graphics/Mesh.h"
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
	bool AnimationStateMachine::GetKeyFrame(AnimationStateMachine::Node* node, const String& boneName, JVector3* T, JQuaternion* Q, JVector3* S)
	{
		AnimationController* animController = GetOwnerAnimationController();
		if (animController == nullptr)
		{
			return false;
		}
		switch (mCurrentNode->NodeType)
		{
		case ENodeType::AnimationClip:
		{
			String currentNodeName = FindNodeName(mCurrentNode);
			SharedPtr<AnimationClip>	 currentAnimClip	  = animController->FindAnimationClip(currentNodeName);
			SharedPtr<AnimationClipInfo> currentAnimClipInfo  = animController->FindAnimationClipInfo(currentNodeName);

			if (currentAnimClip->GetCurrentKeyFrame(boneName, currentAnimClipInfo->TimePos, T, Q, S) == false)
			{
				return false;
			}

			if (mIsTransitioning)
			{
				String prevNodeName = FindNodeName(mPrevNode);
				SharedPtr<AnimationClip>	 prevAnimClip = animController->FindAnimationClip(prevNodeName);
				SharedPtr<AnimationClipInfo> prevAnimClipInfo = animController->FindAnimationClipInfo(prevNodeName);
				JVector3 prev_T; JQuaternion prev_Q;
				JVector3 prev_S;
				if (prevAnimClip->GetCurrentKeyFrame(boneName, prevAnimClipInfo->TimePos, &prev_T, &prev_Q, &prev_S) == false)
				{
					return true;
				}
				if (T) *T = JVector3::Lerp(prev_T, *T, mTransitionFactor);
				if (Q) *Q = JQuaternion::Slerp(prev_Q, *Q, mTransitionFactor);
				if (S) *S = JVector3::Lerp(prev_S, *S, mTransitionFactor);
			}
		}
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
	}

	void AnimationStateMachine::UpdateAnimationClipInfo_Thread()
	{
		AnimationController* animController = GetOwnerAnimationController();
		if (animController == nullptr)
		{
			return;
		}
		f32 tick = Application::GetInstance().GetAppTimer()->GetTick();
		// Current Update
		String currentNodeName = FindNodeName(mCurrentNode);

		SharedPtr<AnimationClipInfo> currentAnimClipInfo = animController->FindAnimationClipInfo(currentNodeName);

		if (currentAnimClipInfo == nullptr)
		{
			JG_LOG_ERROR("%s is not exist Animation Clip", currentNodeName);
			return;
		}

		currentAnimClipInfo->TimePos += tick * currentAnimClipInfo->TickPerSecond * currentAnimClipInfo->Speed * 10;
		if (currentAnimClipInfo->GetFlags() & EAnimationClipFlags::Repeat)
		{
			if (currentAnimClipInfo->TimePos >= currentAnimClipInfo->Duration)
			{
				currentAnimClipInfo->Reset();
			}
		}



		if (mIsTransitioning && mPrevNode != nullptr && mPrevNode->NodeType != ENodeType::Begin)
		{

			String prevNodeName = FindNodeName(mPrevNode);
			SharedPtr<AnimationClipInfo> prevAnimClipInfo = animController->FindAnimationClipInfo(prevNodeName);

			if (prevAnimClipInfo == nullptr)
			{
				JG_LOG_ERROR("%s is not exist Animation Clip", prevNodeName);
				return;
			}

			prevAnimClipInfo->TimePos += tick * prevAnimClipInfo->TickPerSecond * prevAnimClipInfo->Speed * 10;
			if (prevAnimClipInfo->GetFlags() & EAnimationClipFlags::Repeat)
			{
				if (prevAnimClipInfo->TimePos >= prevAnimClipInfo->Duration)
				{
					prevAnimClipInfo->Reset();
				}
			}

			mTransitionTimePos += tick;
			AnimationTransition* transition = FindTransition(prevNodeName, currentNodeName);
			if (transition != nullptr && transition->GetTransitionDuration() < mTransitionTimePos)
			{
				mIsTransitioning = false;
				mPrevNode        = nullptr;
				mTransitionTimePos = 0.0f;
				mTransitionFactor = 1.0f;
				return;
			}

			mTransitionFactor = Math::Min(mTransitionTimePos / transition->GetTransitionDuration(), 1.0f);
		}
		else
		{
			mIsTransitioning = false;
		}


	}

	void AnimationStateMachine::UpdateAnimationTransform_AnimClip_Thread(Node* node, List<SharedPtr<AnimationTransform>>& out_animTransform)
	{
		AnimationController* animController = GetOwnerAnimationController();

		String name = mAnimClipNameList[node->ID];
		SharedPtr<AnimationClip>	 animClip     = animController->FindAnimationClip(name);
		SharedPtr<AnimationClipInfo> animClipInfo = animController->FindAnimationClipInfo(name);
		SharedPtr<IMesh> mesh					  = animController->GetBindedOriginMesh();
		SharedPtr<Skeletone> skeletone			  = animController->GetBindedSkeletone();
		if (animClip == nullptr || animClipInfo == nullptr)
		{
			JG_LOG_ERROR("%s is not exist Animation Clip", name);
			return;
		}

		if (skeletone == nullptr || skeletone->IsValid() == false  || mesh == nullptr)
		{
			return;
		}

		UpdateAnimationClipInfo_Thread();
		if (animClipInfo->TimePos >= animClipInfo->Duration)
		{
			return;
		}

		u32 subMeshCount = mesh->GetSubMeshCount();
		out_animTransform.resize(subMeshCount);
		for (u32 i = 0; i < subMeshCount; ++i)
		{
			out_animTransform[i] = CreateSharedPtr<AnimationTransform>();
			UpdateAnimationTransform_AnimClip_Internal_Thread(mesh->GetSubMesh(i), skeletone->GetRootNodeID(),  JMatrix::Identity(), out_animTransform[i]);
		}
	}

	void AnimationStateMachine::UpdateAnimationTransform_AnimClip_Internal_Thread(SharedPtr<ISubMesh> subMesh, u32 nodeID, const JMatrix& parentTransform, SharedPtr<AnimationTransform> animTransform)
	{
		AnimationController* animController = GetOwnerAnimationController();
		SharedPtr<Skeletone> skeletone = animController->GetBindedSkeletone();


		const Skeletone::Node* node = skeletone->GetNode(nodeID);
		JMatrix nodeTransform = node->Transform;

		JVector3    Scale;
		JQuaternion RotationQ;
		JVector3    Location;

		bool isExistKeyFrame = GetKeyFrame(mCurrentNode, node->Name, &Location, &RotationQ, &Scale);
		if (isExistKeyFrame)
		{
			nodeTransform = JMatrix::AffineTransformation(Location, RotationQ, Scale);
		}


		JMatrix resultTransform = nodeTransform * parentTransform;

		JMatrix boneOffset = JMatrix::Identity();

		if (subMesh->GetBoneOffset(node->ID, &boneOffset) == true)
		{
			JMatrix finalTransform = skeletone->GetRootOffsetTransform() * boneOffset * resultTransform;
			animTransform->Set(node->ID, JMatrix::Transpose(finalTransform));
		}
		for (u32 nodeID : node->ChildNodes)
		{
			const Skeletone::Node* childNode = skeletone->GetNode(nodeID);
			UpdateAnimationTransform_AnimClip_Internal_Thread(subMesh, childNode->ID, resultTransform, animTransform);
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