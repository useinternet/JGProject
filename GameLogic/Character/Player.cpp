#include"Player.h"
#include"Anim/Player/Anim_Player.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"EngineFrameWork/Components/InputComponent.h"
#include"PhysicsSystem/JGBox2D/JGDynamics/JG2DFilter.h"
#include"StaticFilter/StaticCollisionFilter.h"
#include"EngineFrameWork/Components/StaticMesh2DComponent.h"
#include"EngineStatics/JGConstructHelper.h"
#include"EngineStatics/JGLog.h"
using namespace std;
Player::Player()
{


	RegisterObjectID(typeid(this));

	// 애니메이션 별 위치 및 박스 크기 설정
	SPlayerPosByAnim Config;
	Config = { 60.0f,170.0f,0.0f,-85.0f,1.0f,1.0f };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_RightIdle, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_LeftIdle, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_RightJumpDown, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_LeftJumpDown, Config));
	
	Config = { 85.0f,135.0f,0.0f,-85.0f,-1.0f,1.0f  };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_RightMove, Config));
	Config = { 85.0f,135.0f,0.0f,-85.0f, 1.0f,1.0f };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_LeftMove, Config));

	Config = { 60.0f,100.0f,0.0f,-35.0f,1.0f,-1.0f  };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_RightJumpUp, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_LeftJumpUp, Config));

	
	// 충돌체 설정
	GetCollision()->SetAsBox(60.0f, 170.0f);
	GetCollision()->SetComponentLocation(900.0F, 0.0f);
	// 애니메이션 컴포넌트
	AnimPlayer = RegisterComponentInObject<Anim_Player>(TT("Anim_Player"));
	AnimPlayer->SetComponentLocation(0.0f, -85.0f);

	// 자식 추가..
	GetCollision()->AddChild(AnimPlayer);

	// 입력 컴포넌트 추가..
	InputDevice = RegisterComponentInObject<InputComponent>(TT("InputDevice"));

	// 현제 상태 입력
	CurrentPlayerState = EPlayerState::Player_RightIdle;
	PrevPlayerState = EPlayerState::Player_RightIdle;
}

Player::~Player()
{

}

void Player::BeginObject(World* world)
{
	Unit::BeginObject(world);

	// 입력 이벤트 바인딩..
	InputDevice->BindKeyCommand(TT("Right"), EKeyState::Down, std::bind(&Player::RightStart,this));
	InputDevice->BindKeyCommand(TT("Right"), EKeyState::Up, std::bind(&Player::RightEnd, this));
	InputDevice->BindKeyCommand(TT("Left"), EKeyState::Down, std::bind(&Player::LeftStart, this));
	InputDevice->BindKeyCommand(TT("Left"), EKeyState::Up, std::bind(&Player::LeftEnd, this));
	InputDevice->BindKeyCommand(TT("Up"), EKeyState::Down, std::bind(&Player::Jump, this));
}

void Player::Tick(const float DeltaTime)
{
	Unit::Tick(DeltaTime);

	// 속도에 의해 물체 이동
	JGVector2D vel = GetCollision()->GetBody()->GetLinearVelocity();
	float velChange = desiredVel - vel.X();
	float impulse = GetCollision()->GetBody()->GetMass() * velChange;
	JGVector2D vecImpulse(impulse, 0.0f);
	GetCollision()->GetBody()->ApplyLinearImpulse(vecImpulse);

	// 추락중일때.. 방향 전환..
	if (IsFallingDown())
	{
		if (IsRight())
		{
			CurrentPlayerState = Player_RightJumpDown;
		}
		if (IsLeft())
		{
			CurrentPlayerState = Player_LeftJumpDown;
		}
	}
	// 점프하고 떨어지지 않을때..
	if (!IsFalling())
	{
		if (CurrentPlayerState == Player_RightJumpDown)
		{
			CurrentPlayerState = Player_RightIdle;
		}
		if (CurrentPlayerState == Player_LeftJumpDown)
		{
			CurrentPlayerState = Player_LeftIdle;
		}
	}


	// 상황에따른박스 충돌체 크기 변경
	if ( (PrevPlayerState != CurrentPlayerState ))
	{
		SPlayerPosByAnim PrevConfig = m_mBox_Pos_Config[PrevPlayerState];
		SPlayerPosByAnim Config     = m_mBox_Pos_Config[CurrentPlayerState];

		m_CollisionCenter.Set(
			m_CollisionCenter.X() + PrevConfig.hx - Config.hx,
			m_CollisionCenter.Y() + PrevConfig.hy - Config.hy );

		JGVector2D Scale(
			m_CollisionCenter.X() * Config.BoxDeltaScaleX,
			m_CollisionCenter.Y() * Config.BoxDeltaScaleY);

		GetCollision()->SetAsBox(Config.hx, Config.hy, Scale);

		PlayerStateLog();

		PrevPlayerState = CurrentPlayerState;
	}
}
void Player::RightStart()
{
	if (!IsFalling() &&  
		AnimPlayer->GetCurrentState() != Anim_Jump)
	{
		CurrentPlayerState = Player_RightMove;
	}
	else if (IsFallingUp() &&
		AnimPlayer->GetCurrentState() == Anim_Jump)
	{
		CurrentPlayerState = Player_RightJumpUp;
	}
	else if (IsFallingDown() && 
		AnimPlayer->GetCurrentState() == Anim_Jump)
	{
		CurrentPlayerState = Player_RightJumpDown;
	}
	desiredVel = Speed;
}
void Player::RightEnd()
{
	if (!IsFalling() && AnimPlayer->GetCurrentState() != Anim_Jump)
	{
		CurrentPlayerState = Player_RightIdle;
	}
	desiredVel = 0;
}
void Player::LeftStart()
{
	if (!IsFalling() && AnimPlayer->GetCurrentState() != Anim_Jump)
	{
		CurrentPlayerState = Player_LeftMove;
	}
	else if (IsFallingUp() &&
		AnimPlayer->GetCurrentState() == Anim_Jump)
	{
		CurrentPlayerState = Player_LeftJumpUp;
	}
	else if (IsFallingDown() &&
		AnimPlayer->GetCurrentState() == Anim_Jump)
	{
		CurrentPlayerState = Player_LeftJumpDown;
	}
	desiredVel = -Speed;
}
void Player::LeftEnd()
{
	if (!IsFalling() && AnimPlayer->GetCurrentState() != Anim_Jump)
	{
		CurrentPlayerState = Player_LeftIdle;
	}
	desiredVel = 0;
}
void Player::Jump()
{
	if (!IsFalling() && AnimPlayer->GetCurrentState() != Anim_Jump)
	{
		JGVector2D vel(0.0f,0.0f);
		vel.Set(0.0, -GetCollision()->GetBody()->GetMass() * JumpForce);
		GetCollision()->GetBody()->ApplyLinearImpulse(vel);
		if (IsRight())
		{
			CurrentPlayerState = Player_RightJumpUp;
		}
		else
		{
			CurrentPlayerState = Player_LeftJumpUp;
		}
	}
}
EPlayerState Player::GetCurrentPlayerState()
{
	return CurrentPlayerState;
}

void Player::SetCurrentPlayerState(EPlayerState State)
{
	CurrentPlayerState = State;
}

void Player::PlayerStateLog()
{
	switch (CurrentPlayerState)
	{
	case Player_LeftIdle:
		JGLog::Write(ELogLevel::Default, TT("LeftIdle"));
		break;
	case Player_RightIdle:
		JGLog::Write(ELogLevel::Default, TT("RightIdle"));
		break;
	case Player_RightMove:
		JGLog::Write(ELogLevel::Default, TT("RightMove"));
		break;
	case Player_LeftMove:
		JGLog::Write(ELogLevel::Default, TT("LeftMove"));
		break;
	case Player_RightJumpUp:
		JGLog::Write(ELogLevel::Default, TT("RightJumpUp"));
		break;
	case Player_RightJumpDown:
		JGLog::Write(ELogLevel::Default, TT("RightJumpDown"));
		break;
	case Player_LeftJumpUp:
		JGLog::Write(ELogLevel::Default, TT("LeftJumpUp"));
		break;
	case Player_LeftJumpDown:
		JGLog::Write(ELogLevel::Default, TT("LeftJumpDown"));
		break;
	}
}
