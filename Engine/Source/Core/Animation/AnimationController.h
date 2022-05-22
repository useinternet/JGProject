#pragma once

#include "JGCore.h"
#include "AnimationDefines.h"
namespace JG
{
	class AnimationClip;
	class AnimationClipInfo;
	class AnimationBlendSpace1D;
	class AnimationBlendSpace1DInfo;
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
	class AnimationBlendSpace;
	class AnimationBlendSpaceInfo;
	namespace Compute
	{
		class AnimationSkinning;
	}
	// Weight 설정
	// Mask 설정
	// 
	// 일단 레이어 설정
	
	class AnimationController
	{
		friend class JGAnimation;

		String mName;
		Dictionary<String, SharedPtr<AnimationStateMachine>> mAnimationStateMachineDic;
		String mBaseLayer = "Base Layer";

		SharedPtr<AnimationParameters> mAnimParams;
		SharedPtr<AnimationParameters> mAnimParams_Thread;
		SharedPtr<Skeletone> mSkeletone;
		SharedPtr<IMesh> mSkinnedMesh;
		SharedPtr<IMesh> mOriginMesh;
		SharedPtr<Compute::AnimationSkinning> mAnimationSkinning;
		Dictionary<String, SharedPtr<AnimationClip>>     mAnimClips;
		Dictionary<String, SharedPtr<AnimationClipInfo>> mAnimClipInfos;

		Dictionary<String, SharedPtr<AnimationBlendSpace1D>>     mAnimBlendSpace1Ds;
		Dictionary<String, SharedPtr<AnimationBlendSpace1DInfo>> mAnimBlendSpace1DInfos;


		Dictionary<String, SharedPtr<AnimationBlendSpace>>     mAnimBlendSpaces;
		Dictionary<String, SharedPtr<AnimationBlendSpaceInfo>> mAnimBlendSpaceInfos;

		struct ClipCommandData
		{
			enum 
			{
				Command_AnimClip_Add,
				Command_AnimClip_Remove,
				Command_BlendSpace1D_Add,
				Command_BlendSpace1D_Remove,
				Command_BlendSpace_Add,
				Command_BlendSpace_Remove,
			};
			i32    Command;
			String Name;
			SharedPtr<AnimationClip>     Clip;
			SharedPtr<AnimationClipInfo> ClipInfo;
			SharedPtr<AnimationBlendSpace1D>     BlendSpace1D;
			SharedPtr<AnimationBlendSpace1DInfo> BlendSpace1DInfo;

			SharedPtr<AnimationBlendSpace>     BlendSpace;
			SharedPtr<AnimationBlendSpaceInfo> BlendSpaceInfo;
		};
		

		mutable std::mutex mMeshLock;
		mutable std::mutex mSkeletonLock;
		Queue<ClipCommandData>        mCommandDataQueue;
		u64 mWaitFrameCount = 0;
		SharedPtr<AnimationStateFlow> mFlow;
		bool mIsResetStateMachine = false;
	public:
		AnimationController();
		// Clip 찾기
		
		void AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, EAnimationClipFlags flags, bool immediate = false);
		void AddAnimationBlendSpace1D(const String& name, SharedPtr< AnimationBlendSpace1D> blendSpace1D, EAnimationBlendSpace1DFlag flags, bool immediate = false);
		void AddAnimationBlendSpace(const String& name, SharedPtr<AnimationBlendSpace> blendSpace, EAnimationBlendSpaceFlag flags, bool immediate = false);
		void RemoveAnimationClip(const String& name, bool immediate = false);
		void RemoveBlendSpace1D(const String& name, bool immediate = false);
		void RemoveBlendSpace(const String& name, bool immediate = false);
		void BindSkeletone(SharedPtr<Skeletone> skeletone);
		void BindMesh(SharedPtr<IMesh> mesh);
		SharedPtr<AnimationClip>	 FindAnimationClip(const String& name) const;
		SharedPtr<AnimationClipInfo> FindAnimationClipInfo(const String& name);
		SharedPtr<AnimationBlendSpace1D> FindAnimationBlendSpace1D(const String& name) const;
		SharedPtr<AnimationBlendSpace1DInfo> FindAnimationBlendSpace1DInfo(const String& name) const;
		SharedPtr<AnimationBlendSpace> FindAnimationBlendSpace(const String& name) const;
		SharedPtr<AnimationBlendSpaceInfo> FindAnimationBlendSpaceInfo(const String& name) const;



		SharedPtr<Skeletone>		   GetBindedSkeletone()       const;
		SharedPtr<IMesh>			   GetBindedMesh() const;
		SharedPtr<IMesh>			   GetBindedOriginMesh() const;
		SharedPtr<AnimationParameters> GetAnimationParameters()   const;
		SharedPtr<AnimationParameters> GetAnimationParameters_Thread()   const;
		SharedPtr<AnimationStateMachine>   GetAnimationStateMachine() const;
		SharedPtr<AnimationStateMachine>   GetAnimationStateMachine(const String& layerName) const;
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
		void ForEach(const std::function<void(const String&, SharedPtr<AnimationStateMachine>)>& action);

		void Update(SharedPtr<IComputeContext> computeContext);
		void Update_Thread(SharedPtr<IComputeContext> computeContext);
		bool CanUseSkinnedMesh() const;
	};
}