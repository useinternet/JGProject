#pragma once
#include "JGCore.h"

namespace JG
{
	class AnimationController;
	class AnimationTransition;
	class AnimationTransform;
	class AnimationClipInfo;
	class AnimationSequence;


	template<class T>
	using MakeAnimationAction = std::function<void(T* )>;

	using MakeAnimationSequenceAction = MakeAnimationAction<AnimationSequence>;
	using MakeAnimationClipAction	  = MakeAnimationAction<AnimationClipInfo>;
	using MakeTransitionAction		  = MakeAnimationAction<AnimationTransition>;
	class AnimationSequence
	{
		enum class ENodeType
		{
			Begin,
			AnimationClip,
			AnimationSequence,
		};
		struct Node
		{
			u32 ID = -1;
			ENodeType NodeType;

			
			List<Node*> ConnectedNode;
			List<SharedPtr<AnimationTransition>> ConnectedNodeTransitions;
		};

		WeakPtr<AnimationController> mOwnerAnimController;

		Dictionary<String, SharedPtr<Node>> mNodeDic;
		List<SharedPtr<AnimationSequence>>  mAnimSequenceList;
		List<String> mAnimClipNameList;

		String mBeginNodeName;

		Node* mCurrentNode = nullptr;
		bool  mIsMakingSequence = false;
		bool  mIsRunning = false;
	public:
		AnimationSequence(SharedPtr<AnimationController> controller);


		bool IsValid() const;
		SharedPtr<AnimationController> GetOwnerAnimationController() const;

		AnimationSequence& Begin(const String& startNodeName);
		AnimationSequence& MakeSequenceNode(const String& name, const MakeAnimationSequenceAction& makeAction);
		AnimationSequence& MakeAnimationClipNode(const String& name, const MakeAnimationClipAction& makeAction);
		AnimationSequence& ConnectNode(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction);
		void End();
	
		SharedPtr<AnimationTransform> Execute();
	private:
		bool CreateNode(ENodeType nodeType, const String& name);
		bool IsExistNode(const String& name) const;
		AnimationSequence::Node* FindNode(const String& name) const;

		SharedPtr<AnimationTransform> ExecuteInternal(Node* node);


		void Reset();
	};
}