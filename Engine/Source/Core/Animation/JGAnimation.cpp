#include "pch.h"
#include "JGAnimation.h"
#include "AnimationController.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/GraphicsAPI.h"
namespace JG
{
#define ANIMATION_COMMAND_QUEUE_ID (u64)this
#define ANIMATION_CONTEXT_ID 1
	JGAnimation::JGAnimation()
	{
		mAnimBeginFrameSH = Scheduler::GetInstance().ScheduleByFrame(
			0, 0, -1, SchedulePriority::Animation_BeginFrame, SCHEDULE_BIND_FN(&JGAnimation::BeginFrame));
		mAnimEndFrameSH = Scheduler::GetInstance().ScheduleByFrame(
			0, 0, -1, SchedulePriority::Animation_EndFrame, SCHEDULE_BIND_FN(&JGAnimation::EndFrame));

		mAnimThreadSH = nullptr;

		JGGraphics::GetInstance().GetGraphicsAPI()->AllocateCommandQueue(ECommandQueueType::Compute, ANIMATION_COMMAND_QUEUE_ID);
	}

	JGAnimation::~JGAnimation()
	{
		JGGraphics::GetInstance().GetGraphicsAPI()->Flush(ANIMATION_COMMAND_QUEUE_ID);

		if (mAnimThreadSH != nullptr)
		{
			mAnimThreadSH->Reset();
			mAnimThreadSH = nullptr;
		}
		if (mAnimBeginFrameSH != nullptr)
		{
			mAnimBeginFrameSH->Reset();
			mAnimBeginFrameSH = nullptr;
		}
		if (mAnimEndFrameSH != nullptr)
		{
			mAnimEndFrameSH->Reset();
			mAnimEndFrameSH = nullptr;
		}
	}

	void JGAnimation::RegisterAnimationController(SharedPtr<AnimationController> controller)
	{
		mCommandQueue.push(CommandData(controller, CommandData::Command_Add));
	}

	void JGAnimation::UnRegisterAnimatioinController(SharedPtr<AnimationController> controller)
	{
		mCommandQueue.push(CommandData(controller, CommandData::Command_Remove));
	}

	EScheduleResult JGAnimation::BeginFrame()
	{
		// 애니메이션 데이터 갱신 루프 시작

		while (mCommandQueue.empty() == false)
		{
			CommandData data = mCommandQueue.front(); mCommandQueue.pop();
			switch (data.Command)
			{
			case CommandData::Command_Add:
				mRegisteredAnimationControllers.insert(data.AnimController);
				break;
			case CommandData::Command_Remove:
				mRegisteredAnimationControllers.erase(data.AnimController);
				break;
			}
		}

		for (SharedPtr<AnimationController> controller : mRegisteredAnimationControllers)
		{
			if (controller == nullptr) continue;
			controller->Update();
		}

		if (mRegisteredAnimationControllers.empty() == false)
		{
			mAnimThreadSH = Scheduler::GetInstance().ScheduleAsync([this]()
			{
				Update_Thread();
			});
		}
		return EScheduleResult::Continue;
	}
	EScheduleResult JGAnimation::EndFrame()
	{
		if (mAnimThreadSH == nullptr)
		{
			return EScheduleResult::Continue;
		}
		// 애니메이션 루프 완료까지 대기
		while (mAnimThreadSH->GetState() != EScheduleState::Compelete) {}
		mAnimThreadSH->Reset();
		mAnimThreadSH = nullptr;
		return EScheduleResult::Continue;
	}

	void JGAnimation::Update_Thread()
	{
		SharedPtr<IComputeContext> computeContext = 
			JGGraphics::GetInstance().GetGraphicsAPI()->GetComputeContext(ANIMATION_COMMAND_QUEUE_ID, ANIMATION_CONTEXT_ID);

		u64 cnt = mRegisteredAnimationControllers.size();
		for (SharedPtr<AnimationController> controller : mRegisteredAnimationControllers)
		{
			controller->Update_Thread(computeContext);
		}

	
		// 끝날때까지 기다리기
		JGGraphics::GetInstance().GetGraphicsAPI()->SubmitAndFlush(ANIMATION_COMMAND_QUEUE_ID);

	}
}
