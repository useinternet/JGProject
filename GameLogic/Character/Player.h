#pragma once
#include"EngineFrameWork/Object/Unit.h"
enum class EReSizePivot;

// 원인  : FallingBlock()이거떄문..
// Box 크기를 퍼센트로 조절해서 사이즈를 조절하기로 하자.
// 점프도 2번함..


// 플레이어 상태
enum EPlayerState
{
	// 기본
	Player_RightIdle,
	Player_LeftIdle,

	// 이동
	Player_RightMove,
	Player_LeftMove,

	// 점프
	Player_RightJumpDown,
	Player_RightJumpUp,
	Player_LeftJumpDown,
	Player_LeftJumpUp,
	
	// 앉기 일어서기
	Player_RightSit,
	Player_RightStand,
	Player_LeftSit,
	Player_LeftStand,




	// 공격
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
class Player : public Unit
{

private:
	// 애니메이션
	class Anim_Player* AnimPlayer;
	// 입력
	class InputComponent* InputDevice;
	// 플레이어 현재 상태 변수
	EPlayerState PrevPlayerState;
	EPlayerState CurrentPlayerState;
	float desiredVel = 0;

	// 환경 변수..
	float JumpForce = 500.0f;
	float Speed = 500.0f;


	// 애니메이션 위치 변수
	std::map<EPlayerState, SPlayerPosByAnim> m_mBox_Pos_Config;
	JGVector2D m_CollisionCenter;
public:
	Player();
	virtual ~Player();
	virtual void BeginObject(World* world) override;
	virtual void Tick(const float DeltaTime) override;
	void RightStart();
	void RightEnd();
	void LeftStart();
	void LeftEnd();
	void Jump();
	EPlayerState GetCurrentPlayerState();
	void SetCurrentPlayerState(EPlayerState State);
	// 임시..
	void PlayerStateLog();
};