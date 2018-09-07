#include "TestAnim.h"
#include"../EngineStatics/JGConstructHelper.h"
#include"../RenderSystem/JGMaterial/JG2DMesh.h"

TestAnim::TestAnim()
{
	RegisterComponentID(typeid(this));

	IdleAnimation = AddAnimation(Idle,TT("Idle"));
	static JGConstructHelper::AnimationMesh2D IdleAnimationMesh(
		GetDevice(), GetBufferManager(), IdleAnimation->GetComponentName(),
		EPivot::TopLeft, 15, 5, 3, TT("../ManagementFiles/Resource/Breath.png"));
	if (IdleAnimationMesh.Success && IdleAnimation)
	{
		IdleAnimation->SetConstructObject(IdleAnimationMesh.Object);
	}


	RightAnimation = AddAnimation(Right, TT("Right"));
	static JGConstructHelper::AnimationMesh2D RightAnimationMesh(
		GetDevice(), GetBufferManager(), RightAnimation->GetComponentName(),
		EPivot::TopLeft, 15, 5, 3, TT("../ManagementFiles/Resource/Breath.png"));
	if (RightAnimationMesh.Success && RightAnimation)
	{
		RightAnimation->SetConstructObject(RightAnimationMesh.Object);
	}

	LeftAnimation = AddAnimation(Left, TT("Left"));
	static JGConstructHelper::AnimationMesh2D LeftAnimationMesh(
		GetDevice(), GetBufferManager(), LeftAnimation->GetComponentName(),
		EPivot::TopLeft, 23, 5, 5, TT("../ManagementFiles/Resource/Water1.png"));
	if (LeftAnimationMesh.Success && LeftAnimation)
	{
		LeftAnimation->SetConstructObject(LeftAnimationMesh.Object);
	}


	UpAnimation = AddAnimation(Up, TT("Up"));
	static JGConstructHelper::AnimationMesh2D UpAnimationMesh(
		GetDevice(), GetBufferManager(), UpAnimation->GetComponentName(),
		EPivot::TopLeft, 19, 5, 4, TT("../ManagementFiles/Resource/Darkness1.png"));
	if (UpAnimationMesh.Success && UpAnimation)
	{
		UpAnimation->SetConstructObject(UpAnimationMesh.Object);
	}

	DownAnimation = AddAnimation(Down,TT("Down"));
	static JGConstructHelper::AnimationMesh2D DownAnimationMesh(
		GetDevice(),GetBufferManager(), DownAnimation->GetComponentName(),
		EPivot::TopLeft,26,5,6, TT("../ManagementFiles/Resource/Fire3.png"));
	if (DownAnimationMesh.Success)
	{
		DownAnimation->SetConstructObject(DownAnimationMesh.Object);
	}
}

TestAnim::~TestAnim()
{

}
