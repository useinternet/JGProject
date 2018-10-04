#pragma once
#include"EngineFrameWork/Object/Side_Scroll_Unit.h"
enum class EReSizePivot;

// 플레이어 상태 최대로 줄이기..


// 플레이어 상태
enum EPlayerState
{
	Player_Idle,
	Player_Move,
	Player_JumpUp,
	Player_JumpDown,
	Player_SitDown,
	Player_StandUp,
	Player_DefaultAttack,
	Player_DefaultSkill,
	Player_SpecialSkill,
	Player_JumpAttack,
	Player_SitAttack,
	Player_SitSkill
};

// 애니메이션 별 위치 및 박스 크기 설정 구조체
struct SPlayerPosByAnim
{
	float hx;
	float hy;
	float x;
	float y;
	float BoxDeltaScaleX;
	float BoxDeltaScaleY;
};
class Player : public Side_Scroll_Unit
{

private:
	// 애니메이션
	class Anim_Player* AnimPlayer;
	// 입력
	class InputComponent* InputDevice;
	// 플레이어 현재 상태 변수
	EPlayerState PrevPlayerState;
	EPlayerState CurrentPlayerState;
	bool m_bPlayerFix = false;
	// 환경 변수..
	float JumpForce   = 500.0f;
	float Speed       = 500.0f;
	// 애니메이션 위치 변수
	std::map<EPlayerState, SPlayerPosByAnim> m_mBox_Pos_Config;
	JGVector2D m_CollisionCenter;

	// 공격 변수
	class DefaultAttackComponent* defaultAttack;
	class DefaultSkillComponent*  defaultSkill;
	class SpecialSkillComponent*  specialSkill;
	class SitSkillComponent*      sitSkill;
public:
	Player();
	virtual ~Player();
	virtual void BeginObject(World* world) override;
	virtual void Tick(const float DeltaTime) override;



	// 상황 
	EPlayerState GetCurrentPlayerState();
	void SetCurrentPlayerState(EPlayerState State);
	bool IsJumping();
	bool IsSitting();
	bool IsMoving();
	void NotifyAttackComplete();
	// 단축키에 대한 행동
	void Move();
	void Attack(class AttackBaseComponent* com, EPlayerState skill);
	void func_Stop();
	void func_RightMove();
	void func_LeftMove();
	void func_Jump();
	void func_SitDown();
	void func_StandUp();
	void func_DefaultAttack();
	void func_DefaultSkill();
	void func_SpeicalSkill();
	// 임시..
	void PlayerStateLog();
};