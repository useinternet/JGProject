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
	Player_DefaultAttack
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

	// 환경 변수..
	float JumpForce = 500.0f;
	float Speed = 500.0f;
	float AttackDelay = 0.2f;
	// 애니메이션 위치 변수
	std::map<EPlayerState, SPlayerPosByAnim> m_mBox_Pos_Config;
	JGVector2D m_CollisionCenter;

public:
	Player();
	virtual ~Player();
	virtual void BeginObject(World* world) override;
	virtual void Tick(const float DeltaTime) override;
	void Move();
	void Stop();
	void RightMove();
	void LeftMove();
	void Jump();
	void SitDown();
	void StandUp();
	void DefaultAttack();



	EPlayerState GetCurrentPlayerState();
	void SetCurrentPlayerState(EPlayerState State);
	bool IsJumping();
	bool IsSitting();
	// 임시..
	void PlayerStateLog();
	void TempLog();
};