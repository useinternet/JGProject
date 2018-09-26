#pragma once
#include"EngineFrameWork/Object/ExistObject.h"


enum EPlayerState
{
	Player_IdleStart,
	Player_Idle,
	Player_RightMove,
	Player_LeftMove,
	Player_Jump,
	Player_Sit
};


class Player : public ExistObject
{
private:
	class Anim_Player* AnimPlayer;
	class Box2DCollisionComponent* BoxCollision;
	class InputComponent* InputDevice;



	EPlayerState CurrentPlayerState;
	float JumpForce = 10.0f;
	float Speed = 500.0f;
	bool bFalling = false;
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
	EPlayerState GetCurrentPlayerState();
	void SetCurrentPlayerState(EPlayerState State);
};