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
	class JGAnimation : public IGlobalSingleton<JGAnimation>
	{

	public:

	};
}