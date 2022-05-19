#pragma once
#include "JGCore.h"

namespace JG
{
	class AnimationController;
	class AnimationTransition;
	class AnimationTransform;
	class AnimationClipInfo;
	class AnimationBlendSpace1DInfo;
	class AnimationStateMachine;
	class AnimationClip;
	class ISubMesh;

	template<class T>
	using MakeAnimationAction = std::function<void(T* )>;

	using MakeAnimationSequenceAction = MakeAnimationAction<AnimationStateMachine>;
	using MakeAnimationClipAction	  = MakeAnimationAction<AnimationClipInfo>;
	using MakeAnimationBlendSpaceAction = MakeAnimationAction<AnimationBlendSpace1DInfo>;
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
			AnimationBlend1D,
			AnimationBlend2D,
		};
		struct Node
		{
			u32 ID = -1;
			ENodeType NodeType;
			List<Node*> ConnectedNode;
			List<AnimationTransition*> ConnectedNodeTransitions;
		};


		AnimationController* mOwnerAnimController = nullptr;

		Dictionary<String, SharedPtr<Node>> mNodeDic;
		Dictionary<String, SharedPtr<AnimationTransition>> mTransitionDic;



		List<String> mAnimNodeNameList;



		String mBeginNodeName;

		Node* mPrevNode    = nullptr;
		Node* mCurrentNode = nullptr;


		// Animation Transition 간 정보
		bool  mIsTransitioning = false; // Transition 변경 중
		f32 mTransitionTimePos = 0.0f;
		f32 mTransitionFactor  = 1.0f;



		bool  mIsMakingMachine = false; //
		bool  mIsRunning       = false;
		std::atomic_bool mLock = false;


		Stack<Node*>	   mFlowNodeStack_Thread;
		AnimationStateFlow mFlow_Thread;
		List<SharedPtr<AnimationTransform>> mFinalAnimationTransforms;
		List<SharedPtr<AnimationTransform>> mPrevAnimationTransform;
	public:
		AnimationStateMachine(AnimationController* controller);
		bool IsValid() const;
		AnimationController* GetOwnerAnimationController() const;

		AnimationStateMachine& Begin(const String& startNodeName);
		AnimationStateMachine& MakeAnimationClipNode(const String& name, const MakeAnimationClipAction& makeAction);
		AnimationStateMachine& MakeAnimationBlendSpace1DNode(const String& name, const MakeAnimationBlendSpaceAction& makeAction);
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
		void Reset_Thread();
	private:
		bool CreateNode(ENodeType nodeType, const String& name);
		AnimationTransition* CreateTransition(const String& prevName, const String& nextName);
		

		bool GetKeyFrame(Node* node, const String& boneName, JVector3* T, JQuaternion* Q, JVector3* S, bool apply_transtioning = true);




		bool IsExistNode(const String& name) const;
		AnimationStateMachine::Node* FindNode(const String& name) const;
		AnimationTransition* FindTransition(const String& prevName, const String& nextName);


		const String& FindNodeName(Node* node) const;
		void UpdateFlow_Thread();
		void UpdateFlow_Thread(Node* node);
		void UpdateNode_Thread();
		void UpdateNode_Internal_Thread(Node* node);
		void UpdateAnimationClipInfo_Thread(const String& clipName);
		void UpdateAnimationBlend1DInfo_Thread(const String& nodeName);
		void UpdateAnimationBlend2DInfo_Thread(const String& nodeName);
		void UpdateAnimationTransform_Thread(List<SharedPtr<AnimationTransform>>& out_animTransform);
		void UpdateAnimationTransform_Internal_Thread(SharedPtr<ISubMesh> subMesh, u32 nodeID,  const JMatrix& parentTransform, SharedPtr<AnimationTransform> animTransform);
		void ResetNode_Thread(Node* node);



		void ExecuteInternal_Thread(Node* node);
		void Clear();
	};
}