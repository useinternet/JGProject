#include"Player.h"
#include"Anim/Player/Anim_Player.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"EngineFrameWork/Components/InputComponent.h"
#include"EngineStatics/JTimerManager.h"
#include"EngineStatics/JMath/JGMath.h"
#include"EngineStatics/JGLog.h"
using namespace std;
Player::Player()
{


	RegisterObjectID(typeid(this));

	// 애니메이션 별 위치 및 박스 크기 설정
	SPlayerPosByAnim Config;


	Config = { 85.0f,135.0f,0.0f,-85.0f,-1.0f,1.0f  };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_Move, Config));

	Config = { 60.0f,100.0f,0.0f,-35.0f,1.0f,-1.0f  };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_JumpUp, Config));
	Config = { 60.0f,170.0f,0.0f,-85.0f,1.0f,1.0f };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_Idle, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_JumpDown, Config));

	Config = { 60.0f,100.0f,0.0f,-85.0f,1.0f,1.0f };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_SitDown, Config));
	Config = { 60.0f,120.0f,0.0f,-85.0f,1.0f,1.0f };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_StandUp, Config));
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
	CurrentPlayerState = EPlayerState::Player_Idle;
	PrevPlayerState = EPlayerState::Player_Idle;
}

Player::~Player()
{

}
void Player::BeginObject(World* world)
{
	Side_Scroll_Unit::BeginObject(world);

	// 입력 이벤트 바인딩..
	InputDevice->BindKeyCommand(TT("Right"), EKeyState::Down, std::bind(&Player::RightMove,this));
	InputDevice->BindKeyCommand(TT("Right"), EKeyState::Up, std::bind(&Player::Stop, this));
	InputDevice->BindKeyCommand(TT("Left"), EKeyState::Down, std::bind(&Player::LeftMove, this));
	InputDevice->BindKeyCommand(TT("Left"), EKeyState::Up, std::bind(&Player::Stop, this));
	InputDevice->BindKeyCommand(TT("Jump"), EKeyState::Down, std::bind(&Player::Jump, this));
	InputDevice->BindKeyCommand(TT("Sit_Stand"), EKeyState::Down, std::bind(&Player::SitDown, this));
	InputDevice->BindKeyCommand(TT("Sit_Stand"), EKeyState::Up, std::bind(&Player::StandUp, this));
	InputDevice->BindKeyCommand(TT("DefaultAttack"), EKeyState::Down, std::bind(&Player::DefaultAttack, this));
}

void Player::Tick(const float DeltaTime)
{
	Side_Scroll_Unit::Tick(DeltaTime);

	if (!IsFalling() && CurrentPlayerState == Player_JumpDown)
	{
		CurrentPlayerState = Player_Idle;
	}
	if (IsFallingDown())
	{
		CurrentPlayerState = Player_JumpDown;
	}
	// 상황에따른박스 충돌체 크기 변경
	if ( (PrevPlayerState != CurrentPlayerState ) || IsWorking())
	{
	
		SPlayerPosByAnim PrevConfig = m_mBox_Pos_Config[PrevPlayerState];
		SPlayerPosByAnim Config     = m_mBox_Pos_Config[CurrentPlayerState];

		m_CollisionCenter.Set(
			m_CollisionCenter.X() + PrevConfig.hx - Config.hx,
			m_CollisionCenter.Y() + PrevConfig.hy - Config.hy );

		JGVector2D Scale(
			m_CollisionCenter.X() * Config.BoxDeltaScaleX,
			m_CollisionCenter.Y() * Config.BoxDeltaScaleY);
		if (CurrentPlayerState == Player_Move && IsLeft())
		{
			Scale.SetNegativeX();
		}
		GetCollision()->SetAsBox(Config.hx, Config.hy, Scale);

		PlayerStateLog();

		PrevPlayerState = CurrentPlayerState;
	}
}
void Player::Move()
{
	if (!IsJumping())
	{
		CurrentPlayerState = Player_Move;
	}
}
void Player::Stop()
{
	if (!IsJumping() && !IsSitting())
	{
		CurrentPlayerState = Player_Idle;
	}
	SetVelocityX(0.0f);
}
void Player::RightMove()
{
	if (!IsSitting())
	{
		Move();
		SetVelocityX(Speed);
	}
	else
	{
		AnimPlayer->GetAnimation(Anim_SitDown)->Reverse(EReverse::Default);
	}
}
void Player::LeftMove()
{
	if (!IsSitting())
	{
		Move();
		SetVelocityX(-Speed);
	}
	else
	{
		AnimPlayer->GetAnimation(Anim_SitDown)->Reverse(EReverse::RL);
	}
}
void Player::Jump()
{
	if (!IsJumping())
	{
		PushUp(JumpForce);
		CurrentPlayerState = Player_JumpUp;
	}
}
void Player::SitDown()
{
	if (!IsJumping())
	{
		CurrentPlayerState = Player_SitDown;
	}
}
void Player::StandUp()
{
	if (!IsJumping() && CurrentPlayerState == Player_SitDown)
	{
		CurrentPlayerState = Player_StandUp;
	}
}
void Player::DefaultAttack()
{
	static float AccTime = AttackDelay;
	AccTime += GetDeltaTime();
	if (AccTime >= AttackDelay)
	{
		AccTime = 0;
		JGLog::Write(ELogLevel::Default, TT("기본 공격"));
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

bool Player::IsJumping()
{
	if (!IsFalling() &&
		CurrentPlayerState != Player_JumpUp &&
		CurrentPlayerState != Player_JumpDown)
	{
		return false;
	}
	return true;
}
bool Player::IsSitting()
{
	if (CurrentPlayerState == Player_SitDown)
	{
		return true;
	}
	return false;
}

void Player::PlayerStateLog()
{
	//switch (CurrentPlayerState)
	//{
	//case Player_Idle:
	//	JGLog::Write(ELogLevel::Default, TT("평상시"));
	//	break;
	//case Player_Move:
	//	JGLog::Write(ELogLevel::Default, TT("움직임"));
	//	break;
	//case Player_JumpDown:
	//	JGLog::Write(ELogLevel::Default, TT("추락"));
	//	break;
	//case Player_JumpUp:
	//	JGLog::Write(ELogLevel::Default, TT("점프"));
	//	break;
	//}
}
void Player::TempLog()
{
	int num = JGMath::RandomDraw(1, 10);
	JGLog::Write(ELogLevel::Default, TT("숫자 : %d"), num);
}
