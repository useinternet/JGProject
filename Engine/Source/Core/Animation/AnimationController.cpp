#include "pch.h"
#include "AnimationController.h"
#include "JGAnimation.h"
#include "AnimationParameters.h"
#include "AnimationSequence.h"
#include "AnimationTransform.h"
#include "AnimationClip.h"
#include "Graphics/Mesh.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/Compute/AnimationSkinning.h"
#include "Class/Data/Skeletone.h"

namespace JG
{
	void AnimationController::AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, EAnimationClipFlags flags)
	{
		if (animationClip == nullptr || animationClip->IsValid() == false)
		{
			return;
		}
		ClipCommandData data;
		data.Command = ClipCommandData::Command_Add;
		data.Name = name;
		data.Clip = animationClip;
		data.ClipInfo = CreateSharedPtr<AnimationClipInfo>(name, animationClip, flags);
		mClipCommandDataQueue.push(data);
	}

	void AnimationController::RemoveAnimationClip(const String& name)
	{
		ClipCommandData data;
		data.Command = ClipCommandData::Command_Remove;
		data.Name = name;
		data.Clip     = nullptr;
		data.ClipInfo = nullptr;
		mClipCommandDataQueue.push(data);
	}

	void AnimationController::BindSkeletone(SharedPtr<Skeletone> skeletone)
	{
		mSkeletone = skeletone;
	}

	void AnimationController::BindMesh(SharedPtr<IMesh> mesh)
	{
		if (mesh == nullptr)
		{
			return;
		}
		u32 subMeshCnt = mesh->GetSubMeshCount();
		for (u32 i = 0; i < subMeshCnt; ++i)
		{
			SharedPtr<ISubMesh> subMesh = mesh->GetSubMesh(i);
			if (subMesh->GetBoneBuffer() == nullptr)
			{
				return;
			}
		}
		if (mOriginMesh != mesh)
		{
			mSkinnedMesh = nullptr;
		}

		mOriginMesh = mesh;
	}

	SharedPtr<AnimationClip> AnimationController::FindAnimationClip(const String& name) const
	{
		if (mAnimClips.find(name) == mAnimClips.end())
		{
			return nullptr;
		}
		return mAnimClips.at(name);
	}

	SharedPtr<AnimationClipInfo> AnimationController::FindAnimationClipInfo(const String& name)
	{
		if (mAnimClips.find(name) == mAnimClips.end())
		{
			return nullptr;
		}
		return mAnimClipInfos.at(name);
	}

	SharedPtr<Skeletone> AnimationController::GetBindedSkeletone() const
	{
		return mSkeletone;
	}

	SharedPtr<IMesh> AnimationController::GetBindedMesh() const
	{
		return mSkinnedMesh;
	}

	SharedPtr<AnimationParameters> AnimationController::GetAnimationParameters() const
	{
		return mAnimParams;
	}

	SharedPtr<AnimationSequence> AnimationController::GetRootAnimationSequence() const
	{
		return mRootSequence;
	}

	SharedPtr<AnimationTransform> AnimationController::GetFinalTransform() const
	{
		return mFinalTransform;
	}

	void AnimationController::Init()
	{
		mAnimParams   = CreateSharedPtr<AnimationParameters>();
		mRootSequence = CreateSharedPtr<AnimationSequence>(shared_from_this());
	}

	void AnimationController::Update()
	{
		while (mClipCommandDataQueue.empty() == false)
		{
			ClipCommandData commandData = mClipCommandDataQueue.front(); mClipCommandDataQueue.pop();
			switch (commandData.Command)
			{
			case ClipCommandData::Command_Add:
				if (mAnimClips.find(commandData.Name) == mAnimClips.end())
				{
					mAnimClips.emplace(commandData.Name, commandData.Clip);
					mAnimClipInfos.emplace(commandData.Name, commandData.ClipInfo);
				}
				break;
			case ClipCommandData::Command_Remove:
				mAnimClips.erase(commandData.Name);
				mAnimClipInfos.erase(commandData.Name);
				break;
			}
		}
		if (mFinalTransform == nullptr)
		{
			mFinalTransform = CreateSharedPtr<AnimationTransform>();
		}

		mPendingTransform != nullptr ? 
			*mFinalTransform = *mPendingTransform : mFinalTransform->Reset();

	}

	void AnimationController::Update_Thread(SharedPtr<IComputeContext> computeContext)
	{
		if (mSkeletone == nullptr || mSkeletone->IsValid() == false)
		{
			return;
		}
		SharedPtr<AnimationTransform> animTransform = mRootSequence->Execute();

		if (mOriginMesh != nullptr && mOriginMesh->IsValid())
		{
			//SharedPtr<ICopyContext> copyContext = computeContext->QueryInterfaceAsCopyContext();
			//if (mAnimationSkinning == nullptr)
			//{
			//	mAnimationSkinning = CreateUniquePtr<Compute::AnimationSkinning>();
			//}
			//// 스키닝 Mesh 생성
			//if (mSkinnedMesh == nullptr)
			//{
			//	mSkinnedMesh = copyContext->CopyMesh(mOriginMesh);
			//}

			//// 애니메이션 스키닝
			//Compute::AnimationSkinning::Input input;
			//input.AnimTransform = animTransform;
			//input.OriginMesh    = mOriginMesh;
			//input.SkinnedMesh   = mSkinnedMesh;
			//mAnimationSkinning->Execute(computeContext, input);
		}
		
	}
}