#include"Player.h"
#include"Anim/Player/Anim_Player.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"EngineFrameWork/Components/InputComponent.h"
#include"Character/PlayerStatus.h"
#include"Skill/DefaultAttackComponent.h"
#include"Skill/DefaultSkillComponent.h"
#include"Skill/SpecialSkillComponent.h"
#include"Skill/SitSkillComponent.h"
#include"PhysicsSystem/JGBox2D/JGDynamics/JG2DFilter.h"
#include"StaticFilter/StaticCollisionFilter.h"
#include"GameMode/GameModeBase.h"
#include"EngineFrameWork/World/World.h"
#include"EngineFrameWork/Components/Camera2DComponent.h"
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
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_JumpAttack, Config));
	Config = { 60.0f,170.0f,0.0f,-85.0f,1.0f,1.0f };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_Idle, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_JumpDown, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_DefaultAttack, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_DefaultSkill, Config));
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_SpecialSkill, Config));

	Config = { 60.0f,100.0f,0.0f,-85.0f,1.0f,1.0f };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_SitDown, Config));
	Config = { 60.0f,120.0f,0.0f,-85.0f,1.0f,1.0f };
	m_mBox_Pos_Config.insert(pair<EPlayerState, SPlayerPosByAnim>(Player_StandUp, Config));
	// 충돌체 설정
	GetCollision()->SetCategoryFilter(Filter_PlayerCollision);
	GetCollision()->SetMaskFilter(Filter_Ground | Filter_Enemy | Filter_EnemyAttack);
	GetCollision()->SetAsBox(60.0f, 170.0f);
	GetCollision()->SetComponentLocation(900.0F, 0.0f);
	// 애니메이션 컴포넌트
	AnimPlayer = RegisterComponentInObject<Anim_Player>(TT("Anim_Player"));
	AnimPlayer->SetComponentLocation(0.0f, -85.0f);

	 // 자식 추가..
	GetCollision()->AddChild(AnimPlayer);

	// 공격 추가..
	defaultAttack = RegisterComponentInObject<DefaultAttackComponent>(TT("Player_DefaultAttack"));
	defaultSkill = RegisterComponentInObject<DefaultSkillComponent>(TT("Player_DefaultSkill"));
	specialSkill = RegisterComponentInObject<SpecialSkillComponent>(TT("Player_SpecialSkill"));


	// 입력 컴포넌트 추가..
	InputDevice = RegisterComponentInObject<InputComponent>(TT("InputDevice"));
	PlayerCamera = RegisterComponentInObject<Camera2DComponent>(TT("PlayerCamera"));




	// 플레이어 스탯
	playerStatus = RegisterComponentInObject<PlayerStatus>(TT("PlayerStatus"));

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
	InputDevice->BindKeyCommand(TT("Right"), EKeyState::Down, bind(&Player::func_RightMove,this));
	InputDevice->BindKeyCommand(TT("Right"), EKeyState::Up, bind(&Player::func_Stop, this));
	InputDevice->BindKeyCommand(TT("Left"), EKeyState::Down, bind(&Player::func_LeftMove, this));
	InputDevice->BindKeyCommand(TT("Left"), EKeyState::Up, bind(&Player::func_Stop, this));
	InputDevice->BindKeyCommand(TT("Jump"), EKeyState::Down, bind(&Player::func_Jump, this));
	
	InputDevice->BindKeyCommand(TT("DefaultAttack"), EKeyState::Down, bind(&Player::func_DefaultAttack, this));
	InputDevice->BindKeyCommand(TT("DefaultSkill"), EKeyState::Down, bind(&Player::func_DefaultSkill, this));
	InputDevice->BindKeyCommand(TT("SpecialSkill"), EKeyState::Down, bind(&Player::func_SpeicalSkill, this));
	InputDevice->BindKeyCommand(TT("Sit_Stand"), EKeyState::Down, bind(&Player::func_SitDown, this));
	InputDevice->BindKeyCommand(TT("Sit_Stand"), EKeyState::Up, bind(&Player::func_StandUp, this));
	world->SetCurrentViewCamera(PlayerCamera);
	GameModeBase* mode = dynamic_cast<GameModeBase*>(world->GetGameMode());
	if (mode)
	{
		mode->SetPlayer(this);
	}
}
void Player::Tick(const float DeltaTime)
{
	Side_Scroll_Unit::Tick(DeltaTime);
	JGVector2D vector = GetCollision()->GetComponentWorldLocation();
	vector.SetY(vector.Y() - 200.0f);
	PlayerCamera->SetComponentLocation(vector);
	if (IsDead())
	{
		GetCollision()->DestroyCollison();
		return;
	}
	if (playerStatus->GetHp() <= 0)
	{
		CurrentPlayerState = Player_Dead;
	}
	//playerStatus->AddCurrentHp(-50 * DeltaTime);
	if (!IsFalling() && (CurrentPlayerState == Player_JumpDown || CurrentPlayerState == Player_JumpAttack))
	{
		m_bPlayerFix = false;
		CurrentPlayerState = Player_Idle;
	}
	if (IsFallingDown() && CurrentPlayerState != Player_JumpAttack)
	{
		CurrentPlayerState = Player_JumpDown;
	}
	// 상황에따른박스 충돌체 크기 변경
	if ( ((PrevPlayerState != CurrentPlayerState ) || IsWorking()) && !IsDead())
	{
	
		SPlayerPosByAnim PrevConfig = m_mBox_Pos_Config[PrevPlayerState];
		SPlayerPosByAnim Config = m_mBox_Pos_Config[CurrentPlayerState];;


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
		CurrentPlayerState != Player_JumpDown &&
		CurrentPlayerState != Player_JumpAttack)
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
bool Player::IsMoving()
{
	if (CurrentPlayerState == Player_Move)
	{
		return true;
	}
	return false;
}
bool Player::IsDead()
{
	if (CurrentPlayerState == Player_Dead)
	{
		return true;
	}
	return false;
}
void Player::NotifyAttackComplete()
{
	m_bPlayerFix = false;
}
void Player::Move()
{
	if (!IsJumping())
	{
		CurrentPlayerState = Player_Move;
	}
}
void Player::Attack(AttackBaseComponent* com, EPlayerState skill)
{
	if (com->IsEnableAttack())
	{

		CurrentPlayerState = skill;
		JGVector2D location = GetCollision()->GetComponentWorldLocation();
	
		location.SetY(location.Y() - 100.0f);
		if (IsRight())
		{
			location.SetX(location.X() + 100.0f);
			com->Attack(location);
		}
		else
		{
			location.SetX(location.X() - 100.0f);
			com->Attack(location,-1.0f);
		}

		m_bPlayerFix = true;
	}
}
void Player::func_Stop()
{
	if (IsDead()) return;

	if (!IsJumping() && !m_bPlayerFix && !IsSitting())
	{
		CurrentPlayerState = Player_Idle;
	}
	SetVelocityX(0.0f);
}
void Player::func_RightMove()
{
	if (IsDead()) return;
	if (!IsSitting() && !m_bPlayerFix)
	{
		Move();
		SetVelocityX(Speed);
	}
	else
	{
		if (!m_bPlayerFix)
		{
			NotifyCompulsoryRight();
		}
	
	}
}
void Player::func_LeftMove()
{
	if (IsDead()) return;
	if (!IsSitting() &&  !m_bPlayerFix)
	{
		Move();
		SetVelocityX(-Speed);
	}
	else
	{
		if (!m_bPlayerFix)
		{
			NotifyCompulsoryLeft();
		}
	}
}
void Player::func_Jump()
{
	if (IsDead()) return;
	if (!IsJumping() && !m_bPlayerFix)
	{
		PushUp(JumpForce);
		CurrentPlayerState = Player_JumpUp;
	}
}
void Player::func_SitDown()
{
	if (IsDead()) return;
	if (!IsJumping() && !m_bPlayerFix && !IsMoving())
	{
		CurrentPlayerState = Player_SitDown;
	}
}
void Player::func_StandUp()
{
	if (IsDead()) return;
	if (!IsJumping() && CurrentPlayerState == Player_SitDown)
	{
		CurrentPlayerState = Player_StandUp;
	}
}
void Player::func_DefaultAttack()
{
	if (IsDead()) return;
	if (IsMoving() || m_bPlayerFix || IsSitting())
	{
		return;
	}
	else if (IsJumping())
	{
		if (CurrentPlayerState == Player_JumpDown)
		{
			Attack(defaultAttack, Player_JumpAttack);
		}
	}
	else
	{
		Attack(defaultAttack, Player_DefaultAttack);
	}
}
void Player::func_DefaultSkill()
{
	if (IsDead()) return;
	if (IsJumping() || IsMoving() || m_bPlayerFix || IsSitting())
	{
		return;
	}
	else
	{
		Attack(defaultSkill, Player_DefaultSkill);
	}
}
void Player::func_SpeicalSkill()
{
	if (IsDead()) return;
	if (IsJumping() || IsSitting() || IsMoving() || m_bPlayerFix)
	{
		return;
	}
	Attack(specialSkill, Player_SpecialSkill);
}
PlayerStatus* Player::GetStatus()
{
	return playerStatus;
}
void Player::YouDie()
{
	RealDead = true;
}
bool Player::IsDie()
{
	return RealDead;
}
void Player::PlayerStateLog()
{
	switch (CurrentPlayerState)
	{
	case Player_Idle:
		JGLog::Write(ELogLevel::Default, TT("Player_Idle"));
		break;
	case Player_Move:
		JGLog::Write(ELogLevel::Default, TT("Player_Move"));
		break;
	case Player_JumpDown:
		JGLog::Write(ELogLevel::Default, TT("Player_JumpDown"));
		break;
	case Player_JumpUp:
		JGLog::Write(ELogLevel::Default, TT("Player_JumpUp"));
		break;
	case Player_SitDown:
		JGLog::Write(ELogLevel::Default, TT("Player_SitDown"));
		break;
	case Player_StandUp:
		JGLog::Write(ELogLevel::Default, TT("Player_StandUp"));
		break;
	case Player_DefaultAttack:
		JGLog::Write(ELogLevel::Default, TT("Player_DefaultAttack"));
		break;
	case Player_DefaultSkill:
		JGLog::Write(ELogLevel::Default, TT("Player_DefaultSkill"));
		break;
	case Player_SpecialSkill:
		JGLog::Write(ELogLevel::Default, TT("Player_SpecialSkill"));
		break;
	case Player_JumpAttack:
		JGLog::Write(ELogLevel::Default, TT("Player_JumpAttack"));
		break;
	}
}




