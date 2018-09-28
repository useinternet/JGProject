#pragma once
#include"EngineFrameWork/Object/ExistObject.h"

enum class EReSizePivot;

// 플레이어 상태
enum EPlayerState
{
	Player_IdleStart,
	Player_Idle,
	Player_RightMove,
	Player_LeftMove,
	Player_JumpDown,
	Player_JumpUp,
	Player_Sit
};

// 애니메이션 별 위치 및 박스 크기 설정 구조체
struct SPlayerPosByAnim
{
	float hx;
	float hy;
	float x;
	float y;
	EReSizePivot pivot;
};
class Player : public ExistObject
{

private:
	// 애니메이션
	class Anim_Player* AnimPlayer;
	// 충돌체 
	class Box2DCollisionComponent* BoxCollision;
	// 입력
	class InputComponent* InputDevice;

	// 플레이어 현재 상태 변수
	bool bFalling     = false;
	bool bFallingDown = false;
	bool bAnimationChange = false;
	EPlayerState PrevPlayerState;
	EPlayerState CurrentPlayerState;
	float desiredVel = 0;
	float FallingCheck_Y = 0.0f;


	// 환경 변수..
	float JumpForce = 500.0f;
	float Speed = 500.0f;

	// 애니메이션 위치 변수
	std::map<EPlayerState, SPlayerPosByAnim> m_mBox_Pos_Config;

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
	void JumpEnd();
	void SitDownStart();
	void SitDownEnd();
	bool IsFalling();
	EPlayerState GetCurrentPlayerState();
	void SetCurrentPlayerState(EPlayerState State);
};