#pragma once
#include "JGCore.h"


namespace JG
{
	/*
	AnimationClip  
	
	이동,회전,스케일 정보, 현재 프레임에서 변환되어야할 트랜스폼, 애니메이션 Raw한 상태 관리



	AnimationController <- 
	AnimationClip 을 이용해서 서로 연결해주는 클래스
	애니메이션 로직 처리, 보간 작업 여기서 해줌


	JGAnimation Thread에서 하는 일


	Animation Clip Transform 제작
	Animation Logic 및 Animation 간 보간 작업
	완성된 Animation은 동기화 후 등록된 Scene에 Vertex 변경 요청 -> SkeletalRenderObject 넘길때 같이 넘김
	
	*/




	/*
	JGAnimation은 등록된 AnimationController 의 로직 관리를 한다.
	*/
	class AnimationController;
	class AnimationClip;
	class IComputeContext;
	class JGAnimation : public IGlobalSingleton<JGAnimation>
	{

		struct CommandData
		{
			enum 
			{
				Command_Add,
				Command_Remove,
			};
			SharedPtr<AnimationController> AnimController;
			i32 Command = 0;

			CommandData(SharedPtr<AnimationController> controller, i32 command) 
				: AnimController(controller), Command(command) {}
		};



		HashSet<SharedPtr<AnimationController>> mRegisteredAnimationControllers;
		Queue<CommandData> mCommandQueue;

		SharedPtr<ScheduleHandle> mAnimThreadSH;
		SharedPtr<ScheduleHandle> mAnimBeginFrameSH;
		SharedPtr<ScheduleHandle> mAnimEndFrameSH;


	public:
		JGAnimation();
		virtual ~JGAnimation();
		void RegisterAnimationController(SharedPtr<AnimationController> controller);
		void UnRegisterAnimatioinController(SharedPtr<AnimationController> controller);
	private:
		EScheduleResult BeginFrame();
		EScheduleResult EndFrame();
		void Update_Thread();

	};
}