#pragma once
#include"EngineFrameWork/Components/Animation2DSystemComponent.h"


// 이번에는 스태틱 매쉬를 동적으로 해서 텍스쳐를 바꿔서 좌우 반전 하게 만들자..
// right Move와 left move 를 합치자..
// 그다음에 player와 따로 커스텀 플레이어로 분리..( 플레이어 이동하고.. 충돌체 관리)
// 커스텀은 리소스 및 애니메이션..

class Player;
enum EAnimState
{
	Anim_Idle,
	Anim_Move,
	Anim_Jump,
	Anim_SitDown,
	Anim_StandUp,
	Anim_DefaultAttack_1,
	Anim_DefaultAttack_2,
	Anim_DefaultSkill,
	Anim_SpecialSkill,
	Anim_JumpDefaultAttack,
	Anim_SitDefaultAttack_1,
	Anim_SitDefaultAttack_2,
	Anim_SitDownSkill,
	Anim_Dead
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

	// 스탠딩 상태 공격
	class AnimationMesh2DComponent* DefaultAttack_1;
	class AnimationMesh2DComponent* DefaultAttack_2;
	class AnimationMesh2DComponent* DefaultSkill;
	class AnimationMesh2DComponent* SpecialSkill;

	// 점프 상태 공격
	class AnimationMesh2DComponent* JumpDefaultAttack;

	// 앉기 상태 공격
	class AnimationMesh2DComponent* SitDefaultAttack_1;
	class AnimationMesh2DComponent* SitDefaultAttack_2;
	class AnimationMesh2DComponent* SitDownSkill;

	// 죽음
	class AnimationMesh2DComponent* Dead;
	class JTimerHandle* DeadTimerHandle;
	bool  bDeadAnimationEnd = false;

	// 일회성 변수
	float DeadAnimAlpha = 1.0f;
	PlayerState PrevPlayerState = 0xFFFFFFF;
public:
	Anim_Player();
	virtual ~Anim_Player();

	virtual void BeginComponent(World* world) override;
	virtual void Tick(const float DeltaTime) override;
	void ConfigDefault(Player* p, EAnimState state);
	void ConfigSitDown(Player* p);
	void ConfigJump(Player* p);
	void ConfigAttack(Player* p);
	void ConfigSitAttack(Player* p);
	void AttackChangeIdleFromEnd();
	void SitAttackChangeSitDownFromEnd();
	void JumpAttackChangeJumpDownFromEnd();
};