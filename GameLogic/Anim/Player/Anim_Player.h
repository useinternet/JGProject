#pragma once
#include"EngineFrameWork/Components/Animation2DSystemComponent.h"


enum EAnimState
{
	Anim_IdleStart = 0,
	Anim_Idle,
	Anim_RightMove,
	Anim_LeftMove,
	Anim_Jump
};
class Anim_Player : public Animation2DSystemComponent
{

private:
	class AnimationMesh2DComponent* IdleStart;
	class AnimationMesh2DComponent* Idle;
	class AnimationMesh2DComponent* RightMove;
	class AnimationMesh2DComponent* LeftMove;
	class AnimationMesh2DComponent* Jump;
public:
	Anim_Player();
	virtual ~Anim_Player();

	virtual void BeginComponent(World* world) override;
	virtual void Tick(const float DeltaTime) override;

};