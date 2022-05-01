#pragma once

#include "JGCore.h"
#include "AnimationDefines.h"
namespace JG
{
	class AnimationClip;
	class AnimationClipInfo;
	class AnimationSequence;
	class AnimationTransition;
	class AnimationParameters;
	class AnimationTransform;
	class Skeletone;
	class AnimationController : public Enable_Shared_From_This<AnimationController>
	{
		friend class JGAnimation;


		// 클립 이 추가되면 AnimationClipInfo 생성
		SharedPtr<AnimationSequence>   mRootSequence;
		SharedPtr<AnimationParameters> mAnimParams;
		SharedPtr<Skeletone> mSkeletone;
		SharedPtr<AnimationTransform> mFinalTransform;
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

		Queue<ClipCommandData> mClipCommandDataQueue;
		SharedPtr<AnimationTransform> mPendingTransform;
	public:
		// Clip 찾기
		void AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, EAnimationClipFlags flags);
		void RemoveAnimationClip(const String& name);
		void BindSkeletone(SharedPtr<Skeletone> skeletone);


		SharedPtr<AnimationClip>	 FindAnimationClip(const String& name) const;
		SharedPtr<AnimationClipInfo> FindAnimationClipInfo(const String& name);

		SharedPtr<Skeletone>		   GetBindedSkeletone()       const;
		SharedPtr<AnimationParameters> GetAnimationParameters()   const;
		SharedPtr<AnimationSequence>   GetRootAnimationSequence() const;
		SharedPtr<AnimationTransform>  GetFinalTransform() const;
	private:
		void Init();
		void Update();
		void Update_Thread();
	};
}