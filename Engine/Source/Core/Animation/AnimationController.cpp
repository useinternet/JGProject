#include "pch.h"
#include "AnimationController.h"
#include "JGAnimation.h"
#include "AnimationParameters.h"
#include "AnimationStateMachine.h"
#include "AnimationTransform.h"
#include "AnimationClip.h"
#include "AnimationTransition.h"
#include "Class/Asset/Asset.h"
#include "Graphics/Mesh.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/Compute/AnimationSkinning.h"
#include "Class/Data/Skeletone.h"

namespace JG
{
	AnimationController::AnimationController()
	{
		mAnimParams = CreateSharedPtr<AnimationParameters>();
		mAnimParams_Thread = CreateSharedPtr<AnimationParameters>();
		mAnimationStateMachine = CreateSharedPtr<AnimationStateMachine>(this);
		mFlow = CreateSharedPtr<AnimationStateFlow>();
	}
	void AnimationController::AddAnimationClip(const String& name, SharedPtr<AnimationClip> animationClip, EAnimationClipFlags flags, bool immediate)
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

		if (immediate)
		{
			if (mAnimClips.find(data.Name) == mAnimClips.end())
			{
				mAnimClips.emplace(data.Name, data.Clip);
				mAnimClipInfos.emplace(data.Name, data.ClipInfo);
			}
		}
		else
		{
			mClipCommandDataQueue.push(data);
		}

	}

	void AnimationController::RemoveAnimationClip(const String& name, bool immediate)
	{
		ClipCommandData data;
		data.Command = ClipCommandData::Command_Remove;
		data.Name = name;
		data.Clip     = nullptr;
		data.ClipInfo = nullptr;
		
		if (immediate)
		{
			mAnimClips.erase(data.Name);
			mAnimClipInfos.erase(data.Name);
		}
		else
		{
			mClipCommandDataQueue.push(data);
		}
	}

	void AnimationController::BindSkeletone(SharedPtr<Skeletone> skeletone)
	{
		std::lock_guard<std::mutex> lock(mSkeletonLock);
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
		std::lock_guard<std::mutex> lock(mMeshLock);
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
		std::lock_guard<std::mutex> lock(mSkeletonLock);
		return mSkeletone;
	}

	SharedPtr<IMesh> AnimationController::GetBindedMesh() const
	{
		std::lock_guard<std::mutex> lock(mMeshLock);
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

	const AnimationStateFlow& AnimationController::GetAnimationStateFlow() const
	{
		return *mFlow;
	}

	void AnimationController::SetAnimationStock(const AnimationAssetStock& stock)
	{
		SharedPtr<AnimationStateMachine> stateMachine = GetAnimationStateMachine();
		SharedPtr<AnimationParameters> animParameters = GetAnimationParameters();
		stateMachine->Begin(stock.RootName);

		Dictionary<String, EAnimationParameterType> paramTypeDic;
		// Anim Param
		for (auto _pair : stock.Parameters)
		{
			const AnimationAssetStock::ParameterData& data = _pair.second;

			switch (data.Type)
			{
			case EAnimationParameterType::Bool:
				animParameters->SetBool(data.Name, *((bool*)data.Data.data()));
				break;
			case EAnimationParameterType::Float:
				animParameters->SetFloat(data.Name, *((f32*)data.Data.data()));
				break;
			case EAnimationParameterType::Int:
				animParameters->SetInt(data.Name, *((i32*)data.Data.data()));
				break;
			}
			paramTypeDic[data.Name] = data.Type;
		}

		// Anim Clip
		for(const AnimationAssetStock::AnimationClipInfo& clipInfo : stock.AnimClips)
		{
			SharedPtr<Asset<AnimationClip>> clipAsset = AssetDataBase::GetInstance().LoadOriginAssetImmediate<AnimationClip>(clipInfo.AssetPath);
			if (clipAsset == nullptr || clipAsset->IsValid() == false)
			{
				continue;
			}
			AddAnimationClip(clipInfo.Name, clipAsset->Get(), clipInfo.Flags, true);
			stateMachine->MakeAnimationClipNode(clipInfo.Name, nullptr);
		}

		// Transition Info
		for (const AnimationAssetStock::AnimationTransitionInfo& transInfo : stock.TransitionInfos)
		{
			stateMachine->ConnectNode(transInfo.PrevName, transInfo.NextName,
				[&](AnimationTransition* transition)
			{
				transition->SetTransitionDuration(transInfo.TransitionDuration);
				for (const AnimationAssetStock::AnimationTransitionConditionInfo& condInfo : transInfo.Transitions)
				{
					switch (paramTypeDic[condInfo.ParameterName])
					{
					case EAnimationParameterType::Bool:
						transition->AddCondition_Bool(condInfo.ParameterName, *((bool*)condInfo.Data.data()));
						break;
					case EAnimationParameterType::Float:
						transition->AddCondition_Float(condInfo.ParameterName, *((f32*)condInfo.Data.data()), condInfo.Condition);
						break;
					case EAnimationParameterType::Int:
						transition->AddCondition_Int(condInfo.ParameterName, *((i32*)condInfo.Data.data()), condInfo.Condition);
						break;
					}
				}
			});

		}
		stateMachine->End();
	}

	bool AnimationController::IsValid() const
	{
		return GetAnimationStateMachine()->IsLock() == false;
	}

	void AnimationController::Reset()
	{
		mIsResetStateMachine = true;
	}

	SharedPtr<AnimationController> AnimationController::Create(const String& name)
	{
		return CreateSharedPtr<AnimationController>();
	}

	void AnimationController::Init()
	{

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
		*mFlow = GetAnimationStateMachine()->GetAnimationStateFlow_Thread();

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
		if (mIsResetStateMachine == true)
		{
			mAnimationStateMachine->Reset_Thread();
			mIsResetStateMachine = false;
		}
		List<SharedPtr<AnimationTransform>> animTransforms = mAnimationStateMachine->Execute_Thread();
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
			input.OriginMesh = mOriginMesh;
			input.SkinnedMesh = CanUseSkinnedMesh() ? mSkinnedMesh : nullptr;
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