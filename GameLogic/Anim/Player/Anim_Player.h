#pragma once
#include"EngineFrameWork/Components/Animation2DSystemComponent.h"


// 이번에는 스태틱 매쉬를 동적으로 해서 텍스쳐를 바꿔서 좌우 반전 하게 만들자..
// right Move와 left move 를 합치자..
// 그다음에 player와 따로 커스텀 플레이어로 분리..( 플레이어 이동하고.. 충돌체 관리)
// 커스텀은 리소스 및 애니메이션..
enum EAnimState
{
	Anim_Idle,
	Anim_Move,
	Anim_Jump,
	Anim_SitDown,
	Anim_StandUp
};
class Anim_Player : public Animation2DSystemComponent
{
private:
	typedef unsigned int PlayerState;
private:
	class AnimationMesh2DComponent* Idle;
	class AnimationMesh2DComponent* Move;
	class AnimationMesh2DComponent* Jump;
	class AnimationMesh2DComponent* SitDown;
	class AnimationMesh2DComponent* StandUp;

	PlayerState PrevPlayerState = 0xFFFFFFF;
public:
	Anim_Player();
	virtual ~Anim_Player();

	virtual void BeginComponent(World* world) override;
	virtual void Tick(const float DeltaTime) override;



	void ConfigIdle(EReverse reverse);
	void ConfigMove(EReverse reverse);
	void ConfigJumpUp(EReverse reverse);
	void ConfigJumpDown(EReverse reverse);
};