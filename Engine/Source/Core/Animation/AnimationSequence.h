#pragma once
#include "JGCore.h"

namespace JG
{
	class AnimationController;
	class AnimationTransition;
	class AnimationTransform;
	class AnimationClipInfo;
	class AnimationSequence;

	class AnimationSequenceNexus
	{
	private:
		AnimationSequence* Pointer = nullptr;
	public:
		AnimationSequenceNexus(AnimationSequence* Pointer) : Pointer(Pointer) {}


		/*AnimationSequenceNexus MakeSequenceNode(const String& name, const MakeAnimationSequenceAction& makeAction);
		AnimationSequenceNexus MakeAnimationClipNode(const String& clipName, const MakeAnimationClipAction& makeAction);
		AnimationSequenceNexus ConnectSequence(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction);
		AnimationSequenceNexus ConnectAnimationClip(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction);*/
		void End();
	private:
		bool IsValid() const;
	};

	template<class T>
	using MakeAnimationAction = std::function<void(T* )>;

	using MakeAnimationSequenceAction = MakeAnimationAction<AnimationSequence>;
	using MakeAnimationClipAction	  = MakeAnimationAction<AnimationClipInfo>;
	using MakeTransitionAction		  = MakeAnimationAction<AnimationTransition>;
	class AnimationSequence
	{
		enum class ENodeType
		{
			Empty,
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

		Dictionary<String, UniquePtr<Node>> mNodeDic;
		List<UniquePtr<AnimationSequence>> mAnimSequenceList;
		List<String> mAnimClipNameList;

		String mStartNodeName;
		String mEndNodeName;

		bool mIsMakingSequence = false;
	public:
		AnimationSequence(SharedPtr<AnimationController> controller);


		bool IsValid() const;
		SharedPtr<AnimationController> GetOwnerAnimationController() const;

		AnimationSequenceNexus Begin(const String& startNodeName, const String& endNodeName);
		AnimationSequenceNexus MakeSequenceNode(const String& name, const MakeAnimationSequenceAction& makeAction);
		AnimationSequenceNexus MakeAnimationClipNode(const String& name, const MakeAnimationClipAction& makeAction);
		AnimationSequenceNexus ConnectNode(const String& prevName, const String& nextName, const MakeTransitionAction& makeAction);
		
		void End();
		void Reset();

		SharedPtr<AnimationTransform> Execute();
	private:
	
		bool CreateNode(ENodeType nodeType, const String& name);
		bool IsExistNode(const String& name) const;
		AnimationSequence::Node* FindNode(const String& name) const;

	};
}