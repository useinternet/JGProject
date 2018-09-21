#pragma once
#include"../EngineFrameWork/Components/Animation2DSystemComponent.h"

enum AnimationState
{
	Right, Left, Up, Down, Idle
};
class ENGINE_EXPORT TestAnim : public Animation2DSystemComponent
{
private:
	AnimationMesh2DComponent* IdleAnimation = nullptr;
	AnimationMesh2DComponent* RightAnimation = nullptr;
	AnimationMesh2DComponent* LeftAnimation = nullptr;
	AnimationMesh2DComponent* UpAnimation = nullptr;
	AnimationMesh2DComponent* DownAnimation = nullptr;




public:
	TestAnim();
	virtual ~TestAnim();

};