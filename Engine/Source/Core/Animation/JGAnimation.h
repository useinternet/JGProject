#pragma once
#include "JGCore.h"


namespace JG
{
	/*
	AnimationClip  
	
	�̵�,ȸ��,������ ����, ���� �����ӿ��� ��ȯ�Ǿ���� Ʈ������, �ִϸ��̼� Raw�� ���� ����



	AnimationController <- 
	AnimationClip �� �̿��ؼ� ���� �������ִ� Ŭ����
	�ִϸ��̼� ���� ó��, ���� �۾� ���⼭ ����


	JGAnimation Thread���� �ϴ� ��


	Animation Clip Transform ����
	Animation Logic �� Animation �� ���� �۾�
	�ϼ��� Animation�� ����ȭ �� ��ϵ� Scene�� Vertex ���� ��û -> SkeletalRenderObject �ѱ涧 ���� �ѱ�
	
	*/




	/*
	JGAnimation�� ��ϵ� AnimationController �� ���� ������ �Ѵ�.
	*/
	class AnimationController;
	class AnimationClip;
	class JGAnimation : public IGlobalSingleton<JGAnimation>
	{
		List<WeakPtr<AnimationController>> mRegisteredAnimationControllers;
		Queue<WeakPtr<AnimationController>> mAddedRegisterAnimationControllerQueue;

		SharedPtr<ScheduleHandle> mAnimThreadSH;
		SharedPtr<ScheduleHandle> mAnimBeginFrameSH;
		SharedPtr<ScheduleHandle> mAnimEndFrameSH;
	public:
		JGAnimation();
		virtual ~JGAnimation();

		void RegisterAnimationController(SharedPtr<AnimationController> controller);
	private:
		EScheduleResult BeginFrame();
		EScheduleResult EndFrame();
		void Update_Thread();

	};
}