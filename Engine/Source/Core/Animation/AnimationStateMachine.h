#pragma once
#include "JGCore.h"

namespace JG
{
	class AnimationController;
	class AnimationTransition;
	class AnimationTransform;
	class AnimationClipInfo;
	class AnimationStateMachine;


	template<class T>
	using MakeAnimationAction = std::function<void(T* )>;

	using MakeAnimationSequenceAction = MakeAnimationAction<AnimationStateMachine>;
	using MakeAnimationClipAction	  = MakeAnimationAction<AnimationClipInfo>;
	using MakeTransitionAction		  = MakeAnimationAction<AnimationTransition>;
	class AnimationStateFlow
	{
	public:
		List<String> NodeList;
	};
	class AnimationStateMachine
	{
		enum class ENodeType
		{
			Begin,
			AnimationClip,
		};
		struct Node
		{
			u32 ID = -1;
			ENodeType NodeType;

			
			List<Node*> ConnectedNode;
			List<SharedPtr<AnimationTransition>> ConnectedNodeTransitions;
		};

		AnimationController* mOwnerAnimController = nullptr;

		Dictionary<String, SharedPtr<Node>> mNodeDic;
		List<String> mAnimClipNameList;

		String mBeginNodeName;

		Node* mCurrentNode = nullptr;
		bool  mIsMakingMachine = false;
		bool  mIsRunning       = false;
		std::atomic_bool mLock = false;

		AnimationStateFlow mFlow_Thread;
	public:
		AnimationStateMachine(AnimationController* controller);


		bool IsValid() const;
		AnimationController* GetOwnerAnimationController() const;

		AnimationStateMachine& Begin(const String& startNodeName);
		AnimationStateMachine& MakeAnimationClipNode(const String& name, const MakeAnimationClipAction& makeAction);
		AnimationStateMachine& ConnectNode(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction);
		void End();



		// 
		bool IsLock() {
			return mLock;
		}
		void Lock() {
			mLock = true;
		}
		void UnLock() {
			mLock = false;
		}
		const AnimationStateFlow& GetAnimationStateFlow_Thread() const;
		List<SharedPtr<AnimationTransform>> Execute_Thread();
	private:
		bool CreateNode(ENodeType nodeType, const String& name);
		bool IsExistNode(const String& name) const;
		AnimationStateMachine::Node* FindNode(const String& name) const;

		List<SharedPtr<AnimationTransform>> ExecuteInternal_Thread(Node* node);


		void Reset();
	};
}