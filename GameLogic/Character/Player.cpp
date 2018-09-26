#include"Player.h"
#include"Anim/Player/Anim_Player.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"EngineFrameWork/Components/InputComponent.h"
Player::Player()
{
	RegisterObjectID(typeid(this));

	BoxCollision = RegisterComponentInObject<Box2DCollisionComponent>(TT("Player_BoxCollision"));
	BoxCollision->SetAsBox(60.0f, 170.0f);
	BoxCollision->SetBodyType(E2DBodyType::Dynamic);
	BoxCollision->SetComponentLocation(900.0f, 500.0f);

	AnimPlayer = RegisterComponentInObject<Anim_Player>(TT("Anim_Player"));
	AnimPlayer->SetCurrentState(EAnimState::Anim_IdleStart);
	AnimPlayer->SetComponentLocation(0.0f, -85.0f);
	BoxCollision->AddChild(AnimPlayer);

	InputDevice = RegisterComponentInObject<InputComponent>(TT("InputDevice"));
	CurrentPlayerState = EPlayerState::Player_IdleStart;
}

Player::~Player()
{

}

void Player::BeginObject(World* world)
{
	ExistObject::BeginObject(world);
	InputDevice->BindKeyCommand(TT("Right"), EKeyState::Down, std::bind(&Player::RightStart,this));
	InputDevice->BindKeyCommand(TT("Right"), EKeyState::Up, std::bind(&Player::RightEnd, this));
	InputDevice->BindKeyCommand(TT("Left"), EKeyState::Down, std::bind(&Player::LeftStart, this));
	InputDevice->BindKeyCommand(TT("Left"), EKeyState::Up, std::bind(&Player::LeftEnd, this));
	InputDevice->BindKeyCommand(TT("Up"), EKeyState::Down, std::bind(&Player::Jump, this));

	BoxCollision->GetBody()->PhysicsOn();
}

void Player::Tick(const float DeltaTime)
{
	ExistObject::Tick(DeltaTime);
	if (AnimPlayer->GetAnimation(EAnimState::Anim_IdleStart)->AnimationIsEnd() && CurrentPlayerState == Player_IdleStart)
	{
		CurrentPlayerState = Player_Idle;
	}

	JGVector2D vel = BoxCollision->GetBody()->GetLinearVelocity();
	float desiredVel = 0;
	switch (CurrentPlayerState)
	{
	case Player_LeftMove:  desiredVel = -Speed; break;
	case Player_Idle:  desiredVel = 0; break;
	case Player_RightMove: desiredVel = Speed; break;
	}
	float velChange = desiredVel - vel.X();
	float impulse = BoxCollision->GetBody()->GetMass() * velChange;
	vel.Set(impulse, 0.0f);
	BoxCollision->GetBody()->ApplyLinearImpulse(vel);

}

void Player::RightStart()
{
	if (!bFalling)
	{
		CurrentPlayerState = Player_RightMove;
	}
}
void Player::RightEnd()
{
	if (!bFalling)
	{
		CurrentPlayerState = Player_Idle;
	}
}
void Player::LeftStart()
{
	if (!bFalling)
	{
		CurrentPlayerState = Player_LeftMove;
	}
}
void Player::LeftEnd()
{
	if (!bFalling)
	{
		CurrentPlayerState = Player_Idle;
	}
}
void Player::Jump()
{
	if (!bFalling)
	{
		CurrentPlayerState = Player_Jump;
	}
}

void Player::JumpEnd()
{
	bFalling = false;
}

void Player::SitDownStart()
{

}

void Player::SitDownEnd()
{
}

EPlayerState Player::GetCurrentPlayerState()
{
	return CurrentPlayerState;
}

void Player::SetCurrentPlayerState(EPlayerState State)
{
	CurrentPlayerState = State;
}
