#include "pch.h"
#include "JGAnimation.h"
#include "AnimationController.h"

namespace JG
{
	JGAnimation::JGAnimation()
	{
		mAnimBeginFrameSH = Scheduler::GetInstance().ScheduleByFrame(
			0, 0, -1, SchedulePriority::Animation_BeginFrame, SCHEDULE_BIND_FN(&JGAnimation::BeginFrame));
		mAnimEndFrameSH = Scheduler::GetInstance().ScheduleByFrame(
			0, 0, -1, SchedulePriority::Animation_EndFrame, SCHEDULE_BIND_FN(&JGAnimation::EndFrame));

		mAnimThreadSH = nullptr;
	}

	JGAnimation::~JGAnimation()
	{
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
		controller->Init();
		mAddedRegisterAnimationControllerQueue.push(controller);
	}

	EScheduleResult JGAnimation::BeginFrame()
	{
		// 애니메이션 데이터 갱신 루프 시작

		while (mAddedRegisterAnimationControllerQueue.empty() == false)
		{
			WeakPtr<AnimationController> controller = mAddedRegisterAnimationControllerQueue.front();
			mAddedRegisterAnimationControllerQueue.pop();
			if (controller.lock() == nullptr)
			{
				continue;
			}
			mRegisteredAnimationControllers.push_back(controller);
		}

		for (WeakPtr<AnimationController> weak_controller : mRegisteredAnimationControllers)
		{
			SharedPtr<AnimationController> controller = weak_controller.lock();
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
		u64 cnt = mRegisteredAnimationControllers.size();

		for (u64 i = 0; i < cnt;)
		{
			SharedPtr<AnimationController> controller = mRegisteredAnimationControllers[i].lock();
			if (controller == nullptr)
			{
				std::swap(mRegisteredAnimationControllers.end() - 1, mRegisteredAnimationControllers.begin() + i);
				mRegisteredAnimationControllers.pop_back();
				--cnt;
			}
			else
			{
				controller->Update_Thread();
				++i;
			}
		}
	}



}
