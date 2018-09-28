#include"Player.h"
#include"Anim/Player/Anim_Player.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"EngineFrameWork/Components/InputComponent.h"
#include"PhysicsSystem/JGBox2D/JGDynamics/JG2DFilter.h"
#include"StaticFilter/StaticCollisionFilter.h"
#include"EngineStatics/JGLog.h"
using namespace std;
Player::Player()
{
	RegisterObjectID(typeid(this));
	// 애니메이션 별 위치 및 박스 크기 설정
	SPlayerPosByAnim Config;
	Config = { 60.0f,170.0f,0.0f,-85.0f ,EReSizePivot::Down};
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_IdleStart, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_Idle, Config));
	Config = { 60.0f,160.0f,0.0f,-80.0f ,EReSizePivot::Middle };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_JumpDown, Config));
	
	Config = { 60.0f,120.0f,0.0f,-125.0f,EReSizePivot::Down };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_RightMove, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_LeftMove, Config));

	Config = { 60.0f,100.0f,0.0f,-35.0f ,EReSizePivot::Up};
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_JumpUp, Config));
	

	
	// 충돌체 설정
	BoxCollision = RegisterComponentInObject<Box2DCollisionComponent>(TT("Player_BoxCollision"));
	BoxCollision->SetAsBox(60.0f, 170.0f);
	BoxCollision->SetBodyType(E2DBodyType::Dynamic);
	BoxCollision->SetComponentLocation(900.0f, 0.0f);




	// 애니메이션 컴포넌트
	AnimPlayer = RegisterComponentInObject<Anim_Player>(TT("Anim_Player"));
	AnimPlayer->SetCurrentState(EAnimState::Anim_IdleStart);
	AnimPlayer->SetComponentLocation(0.0f, -85.0f);

	// 자식 추가..
	BoxCollision->AddChild(AnimPlayer);

	// 입력 컴포넌트 추가..
	InputDevice = RegisterComponentInObject<InputComponent>(TT("InputDevice"));

	// 현제 상태 입력
	CurrentPlayerState = EPlayerState::Player_IdleStart;
	PrevPlayerState = EPlayerState::Player_IdleStart;
}

Player::~Player()
{

}

void Player::BeginObject(World* world)
{
	ExistObject::BeginObject(world);

	// 입력 이벤트 바인딩..
	InputDevice->BindKeyCommand(TT("Right"), EKeyState::Down, std::bind(&Player::RightStart,this));
	InputDevice->BindKeyCommand(TT("Right"), EKeyState::Up, std::bind(&Player::RightEnd, this));
	InputDevice->BindKeyCommand(TT("Left"), EKeyState::Down, std::bind(&Player::LeftStart, this));
	InputDevice->BindKeyCommand(TT("Left"), EKeyState::Up, std::bind(&Player::LeftEnd, this));
	InputDevice->BindKeyCommand(TT("Up"), EKeyState::Down, std::bind(&Player::Jump, this));
	InputDevice->BindKeyCommand(TT("Up"), EKeyState::Up, std::bind(&Player::JumpEnd, this));
	// 물리 끄고 키기..
	BoxCollision->GetBody()->PhysicsOn();
	BoxCollision->GetBody()->origin()->SetFixedRotation(true);
}

void Player::Tick(const float DeltaTime)
{
	ExistObject::Tick(DeltaTime);
	if (AnimPlayer->GetAnimation(EAnimState::Anim_IdleStart)->AnimationIsEnd() && CurrentPlayerState == Player_IdleStart)
	{
		CurrentPlayerState = Player_Idle;
	}
	// 속도에 의해 물체 이동
	JGVector2D vel = BoxCollision->GetBody()->GetLinearVelocity();
	JGLog::Write(ELogLevel::Default, TT("x: %d , y : %d "), (int)vel.X(), (int)vel.Y());
	float velChange = desiredVel - vel.X();
	float impulse = BoxCollision->GetBody()->GetMass() * velChange;
	JGVector2D vecImpulse(impulse, 0.0f);
	BoxCollision->GetBody()->ApplyLinearImpulse(vecImpulse);
	float DeltaY = BoxCollision->GetComponentLocalLocation().Y() - FallingCheck_Y;
 
	if (DeltaY != 0 && !bAnimationChange)
	{
		JGLog::Write(ELogLevel::Default, TT("떨어지는중 "));
		bFalling = true;
		if (DeltaY > 0)
		{
			CurrentPlayerState = Player_JumpDown;
		}
	}
	if (DeltaY == 0 && !bAnimationChange)
	{
		bFallingDown = false;
		JGLog::Write(ELogLevel::Default, TT("착지"));
		bFalling = false;
	}
	if(bAnimationChange)
	{
		bAnimationChange = false;
	}
	// 현제 상태와 전 상태가 다르다면 충돌체 박스크기를 변경한다.
	if (PrevPlayerState != CurrentPlayerState)
	{
		SPlayerPosByAnim Config = m_mBox_Pos_Config[CurrentPlayerState];
		AnimPlayer->SetComponentLocation(Config.x, Config.y);
		BoxCollision->SetAsBox(Config.hx, Config.hy, Config.pivot);
		PrevPlayerState = CurrentPlayerState;

		if (CurrentPlayerState == Player_JumpDown)
		{
			BoxCollision->AddComponentLocation(0.0f, 45.0f);
			JGVector2D location = BoxCollision->GetComponentWorldLocation();
			JGAngle2D angle = BoxCollision->GetComponentWorldAngle();
			BoxCollision->GetBody()->SetTransform(location, angle);
		}


		bAnimationChange = true;
	}
	FallingCheck_Y = BoxCollision->GetComponentLocalLocation().Y();

}
void Player::RightStart()
{
	if (!bFalling && AnimPlayer->GetCurrentState() != Anim_Jump)
	{
		CurrentPlayerState = Player_RightMove;
	}

	desiredVel = Speed;
}
void Player::RightEnd()
{
	if (!bFalling && AnimPlayer->GetCurrentState() != Anim_Jump)
	{
		CurrentPlayerState = Player_Idle;
	}

	desiredVel = 0;
}
void Player::LeftStart()
{
	if (!bFalling && AnimPlayer->GetCurrentState() != Anim_Jump)
	{
		CurrentPlayerState = Player_LeftMove;
	}
	desiredVel = -Speed;
}
void Player::LeftEnd()
{
	if (!bFalling && AnimPlayer->GetCurrentState() != Anim_Jump)
	{
		CurrentPlayerState = Player_Idle;
	}
	desiredVel = 0;
}
void Player::Jump()
{
	if (!bFalling && !AnimPlayer->GetAnimation(Anim_Jump)->IsPlaying())
	{
		JGVector2D vel(0.0, -BoxCollision->GetBody()->GetMass() * JumpForce);
		BoxCollision->GetBody()->ApplyLinearImpulse(vel);
		CurrentPlayerState = Player_JumpUp;
	}
}

void Player::JumpEnd()
{

}

void Player::SitDownStart()
{

}

void Player::SitDownEnd()
{
}

bool Player::IsFalling()
{
	return bFalling;
}

EPlayerState Player::GetCurrentPlayerState()
{
	return CurrentPlayerState;
}

void Player::SetCurrentPlayerState(EPlayerState State)
{
	CurrentPlayerState = State;
}