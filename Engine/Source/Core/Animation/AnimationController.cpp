#include "pch.h"
#include "AnimationController.h"
#include "JGAnimation.h"
#include "AnimationParameters.h"
#include "AnimationStateMachine.h"
#include "AnimationTransform.h"
#include "AnimationClip.h"
#include "Class/Asset/Asset.h"
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
		return CanUseSkinnedMesh() ? mSkinnedMesh : mOriginMesh;
	}

	SharedPtr<IMesh> AnimationController::GetBindedOriginMesh() const
	{
		return mOriginMesh;
	}

	SharedPtr<AnimationParameters> AnimationController::GetAnimationParameters() const
	{
		return mAnimParams;
	}

	SharedPtr<AnimationParameters> AnimationController::GetAnimationParameters_Thread() const
	{
		return mAnimParams_Thread;
	}

	SharedPtr<AnimationStateMachine> AnimationController::GetAnimationStateMachine() const
	{
		return mAnimationStateMachine;
	}

	void AnimationController::Init()
	{
		mAnimParams   = CreateSharedPtr<AnimationParameters>();
		mAnimParams_Thread = CreateSharedPtr<AnimationParameters>();
		mAnimationStateMachine = CreateSharedPtr<AnimationStateMachine>(this);
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

		*mAnimParams_Thread = *mAnimParams;


		// 스키닝 Mesh 생성
		SharedPtr<IGraphicsContext> context = JGGraphics::GetInstance().GetGraphicsAPI()->GetGraphicsContext();
		SharedPtr<ICopyContext> copyContext = context->QueryInterfaceAsCopyContext();

		if (mSkinnedMesh == nullptr)
		{
			mWaitFrameCount = 0;
			mSkinnedMesh = copyContext->CopyMesh(mOriginMesh);
		}
		else if (mSkinnedMesh != nullptr && CanUseSkinnedMesh() == false)
		{
			++mWaitFrameCount;
		}
	}


	void AnimationController::Update_Thread(SharedPtr<IComputeContext> computeContext)
	{
		if (mSkeletone == nullptr || mSkeletone->IsValid() == false)
		{
			return;
		}
		List<SharedPtr<AnimationTransform>> animTransforms = mAnimationStateMachine->Execute();
		if (mOriginMesh != nullptr && mOriginMesh->IsValid())
		{
			SharedPtr<ICopyContext> copyContext = computeContext->QueryInterfaceAsCopyContext();
			if (mAnimationSkinning == nullptr)
			{
				mAnimationSkinning = CreateUniquePtr<Compute::AnimationSkinning>();
			}

			// 애니메이션 스키닝
			Compute::AnimationSkinning::Input input;
			input.AnimTransforms = animTransforms;
			input.OriginMesh     = mOriginMesh;
			input.SkinnedMesh    = CanUseSkinnedMesh() ? mSkinnedMesh : nullptr;
			mAnimationSkinning->Execute(computeContext, input);
		}
	}
	bool AnimationController::CanUseSkinnedMesh() const
	{
		return
			mSkinnedMesh != nullptr && mSkinnedMesh->IsValid() && 
			mWaitFrameCount >= JGGraphics::GetInstance().GetBufferCount() &&
			mSkeletone != nullptr && mSkeletone->IsValid();
	}
}