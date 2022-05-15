#pragma once

#include "JGCore.h"
#include "AnimationDefines.h"
namespace JG
{
	class AnimationClip;
	class AnimationClipInfo;
	class AnimationStateMachine;
	class AnimationTransition;
	class AnimationParameters;
	class AnimationTransform;
	class Skeletone;
	class IMesh;
	class IComputeContext;
	class IReadBackBuffer;
	class AnimationAssetStock;
	class AnimationStateFlow;
	namespace Compute
	{
		class AnimationSkinning;
	}
	
	class AnimationController
	{
		friend class JGAnimation;

		String mName;
		// 클립 이 추가되면 AnimationClipInfo 생성
		SharedPtr<AnimationStateMachine>   mAnimationStateMachine;
		SharedPtr<AnimationParameters> mAnimParams;
		SharedPtr<AnimationParameters> mAnimParams_Thread;
		SharedPtr<Skeletone> mSkeletone;
		SharedPtr<IMesh> mSkinnedMesh;
		SharedPtr<IMesh> mOriginMesh;
		SharedPtr<Compute::AnimationSkinning> mAnimationSkinning;
		Dictionary<String, SharedPtr<AnimationClip>>     mAnimClips;
		Dictionary<String, SharedPtr<AnimationClipInfo>> mAnimClipInfos;

		struct ClipCommandData
		{
			enum 
			{
				Command_Add,
				Command_Remove,
			};
			i32    Command;
			String Name;
			SharedPtr<AnimationClip>     Clip;
			SharedPtr<AnimationClipInfo> ClipInfo;
		};
		

		mutable std::mutex mMeshLock;
		mutable std::mutex mSkeletonLock;
		Queue<ClipCommandData>        mClipCommandDataQueue;
		u64 mWaitFrameCount = 0;
		SharedPtr<AnimationStateFlow> mFlow;
		bool mIsResetStateMachine = false;
	public:
		AnimationController();
		// Clip 찾기
		void AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, EAnimationClipFlags flags, bool immediate = false);
		void RemoveAnimationClip(const String& name, bool immediate = false);
		void BindSkeletone(SharedPtr<Skeletone> skeletone);
		void BindMesh(SharedPtr<IMesh> mesh);
		SharedPtr<AnimationClip>	 FindAnimationClip(const String& name) const;
		SharedPtr<AnimationClipInfo> FindAnimationClipInfo(const String& name);

		SharedPtr<Skeletone>		   GetBindedSkeletone()       const;
		SharedPtr<IMesh>			   GetBindedMesh() const;
		SharedPtr<IMesh>			   GetBindedOriginMesh() const;
		SharedPtr<AnimationParameters> GetAnimationParameters()   const;
		SharedPtr<AnimationParameters> GetAnimationParameters_Thread()   const;
		SharedPtr<AnimationStateMachine>   GetAnimationStateMachine() const;

		const AnimationStateFlow& GetAnimationStateFlow() const;

		void SetAnimationStock(const AnimationAssetStock& stock);
		
		void SetName(const String& name) {
			mName = name;
		}
		const String& GetName() const {
			return mName;
		}
		bool IsValid() const;
		
		void Reset();
		static SharedPtr<AnimationController> Create(const String& name);
	private:
		void Init();
		void Update();
		void Update_Thread(SharedPtr<IComputeContext> computeContext);
		bool CanUseSkinnedMesh() const;
	};
}