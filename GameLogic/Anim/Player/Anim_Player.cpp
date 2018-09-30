#include"Anim_Player.h"
#include"EngineFrameWork/Components/AnimationMesh2DComponent.h"
#include"RenderSystem/JGMaterial/JG2DMesh.h"
#include"EngineStatics/JGConstructHelper.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"Character/Player.h"


using namespace std;
Anim_Player::Anim_Player()
{
	Idle = AddAnimation(EAnimState::Anim_Idle, TT("Anim_Player_Idle"));
	if (Idle)
	{
		static JGConstructHelper::AnimationMesh2D Anim_Idle_Mesh(
			Idle->GetComponentName(), EPivot::MiddleMiddle, 8, 4, 2,
			TT("../Contents/Player/Idle1-2/AnimSheet_Idle 1-2.png"),
			EReverse::Default,EJGUsageType::Dynamic);
		if (Anim_Idle_Mesh.Success)
		{
			Idle->SetConstructObject(Anim_Idle_Mesh.Object);
		}
		Idle->AnimationSetDelay(0.1f);
	}

	Move = AddAnimation(EAnimState::Anim_Move, TT("Anim_Player_Move"));
	if (Move)
	{
		static JGConstructHelper::AnimationMesh2D Anim_LeftMove_Mesh(
			Move->GetComponentName(), EPivot::MiddleMiddle,6,3,2,
			TT("../Contents/Player/Move/AnimSheet_Move.png"),EReverse::Default,
			EJGUsageType::Dynamic);
		if (Anim_LeftMove_Mesh.Success)
		{
			Move->SetConstructObject(Anim_LeftMove_Mesh.Object);
		}
		Move->AnimationSetDelay(0.1f);
	}

	Jump = AddAnimation(EAnimState::Anim_Jump, TT("Anim_Player_Jump"));
	if (Jump)
	{
		static JGConstructHelper::AnimationMesh2D Anim_Jump_Mesh(
			Jump->GetComponentName(), EPivot::MiddleMiddle, 7, 4, 2,
			TT("../Contents/Player/Jump/AnimSheet_Player_Jump.png"),
			EReverse::Default,EJGUsageType::Dynamic);
		if (Anim_Jump_Mesh.Success)
		{
			Jump->SetConstructObject(Anim_Jump_Mesh.Object);
		}
		Jump->AnimationSetDelay(0.1f);
	}
	SitDown = AddAnimation(EAnimState::Anim_SitDown, TT("Anim_Player_SitDown"));
	if (SitDown)
	{
		static JGConstructHelper::AnimationMesh2D Anim_SitDown_Mesh(
			SitDown->GetComponentName(), EPivot::MiddleMiddle, 4, 4, 1,
			TT("../Contents/Player/Sit/AnimSheet_Player_SitDown.png"),
			EReverse::Default, EJGUsageType::Dynamic);
		if (Anim_SitDown_Mesh.Success)
		{
			SitDown->SetConstructObject(Anim_SitDown_Mesh.Object);
		}
		SitDown->AnimationSetDelay(0.1f);
	}
	StandUp = AddAnimation(EAnimState::Anim_StandUp, TT("Anim_Player_StandUp"));
	if (StandUp)
	{
		static JGConstructHelper::AnimationMesh2D Anim_StandUp_Mesh(
			StandUp->GetComponentName(), EPivot::MiddleMiddle,4,4,1,
			TT("../Contents/Player/Sit/AnimSheet_Player_StandUp.png"),
			EReverse::Default, EJGUsageType::Dynamic);
		if (Anim_StandUp_Mesh.Success)
		{
			StandUp->SetConstructObject(Anim_StandUp_Mesh.Object);
		}
		StandUp->AnimationSetDelay(0.1f);
	}
	SetCurrentState(Anim_Idle);
}

Anim_Player::~Anim_Player()
{


}

void Anim_Player::BeginComponent(World* world)
{
	Animation2DSystemComponent::BeginComponent(world);

	AddNotifyEvent(TT("Frame_Stop_by_Jump_FallingUp"), Anim_Jump, 3, [this]()
	{
		this->GetAnimation(Anim_Jump)->Stop();
	});
	AddNotifyEvent(TT("Frame_Stop_by_Jump_FallingDown"), Anim_Jump, 7, [this]()
	{
		this->GetAnimation(Anim_Jump)->Stop();
	});
}

void Anim_Player::Tick(const float DeltaTime)
{
	Animation2DSystemComponent::Tick(DeltaTime);


	Player* p = dynamic_cast<Player*>(GetOwnerObject());

	if (p)
	{
		if (PrevPlayerState != p->GetCurrentPlayerState())
		{
			switch (p->GetCurrentPlayerState())
			{
			case EPlayerState::Player_RightIdle:
				ConfigIdle(EReverse::Default);
				break;
			case EPlayerState::Player_LeftIdle:
				ConfigIdle(EReverse::RL);
				break;
			case EPlayerState::Player_RightJumpUp:
				ConfigJumpUp(EReverse::Default);
				break;
			case EPlayerState::Player_LeftJumpUp:
				ConfigJumpUp(EReverse::RL);
				break;
			case EPlayerState::Player_RightJumpDown:
				ConfigJumpDown(EReverse::Default);
				break;
			case EPlayerState::Player_LeftJumpDown:
				ConfigJumpDown(EReverse::RL);
				break;
			case EPlayerState::Player_LeftMove:
				ConfigMove(EReverse::RL);
				break;
			case EPlayerState::Player_RightMove:
				ConfigMove(EReverse::Default);
				break;
			}
		}
		PrevPlayerState = p->GetCurrentPlayerState();
	}
	


}

void Anim_Player::ConfigIdle(EReverse reverse)
{
	GetAnimation(Anim_Idle)->Reverse(reverse);
	SetCurrentState(Anim_Idle);
}

void Anim_Player::ConfigMove(EReverse reverse)
{
	GetAnimation(Anim_Move)->Reverse(reverse);
	SetCurrentState(Anim_Move);
}

void Anim_Player::ConfigJumpUp(EReverse reverse)
{
	GetAnimation(Anim_Jump)->Reverse(reverse);
	SetCurrentState(Anim_Jump);
}

void Anim_Player::ConfigJumpDown(EReverse reverse)
{
	GetAnimation(Anim_Jump)->Reverse(reverse);
	SetCurrentState(Anim_Jump);
	GetAnimation(Anim_Jump)->SetCurrentFrame(4);
	GetAnimation(Anim_Jump)->Play();
}
