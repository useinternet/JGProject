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
		bool  mIsMakingSequence = false;
		bool  mIsRunning = false;
	public:
		AnimationStateMachine(AnimationController* controller);


		bool IsValid() const;
		AnimationController* GetOwnerAnimationController() const;

		AnimationStateMachine& Begin(const String& startNodeName);
		AnimationStateMachine& MakeAnimationClipNode(const String& name, const MakeAnimationClipAction& makeAction);
		AnimationStateMachine& ConnectNode(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction);
		void End();
	
		List<SharedPtr<AnimationTransform>> Execute();
	private:
		bool CreateNode(ENodeType nodeType, const String& name);
		bool IsExistNode(const String& name) const;
		AnimationStateMachine::Node* FindNode(const String& name) const;

		List<SharedPtr<AnimationTransform>> ExecuteInternal(Node* node);


		void Reset();
	};
}