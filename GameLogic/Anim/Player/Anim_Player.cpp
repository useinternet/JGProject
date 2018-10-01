#include"Anim_Player.h"
#include"EngineFrameWork/Components/AnimationMesh2DComponent.h"
#include"RenderSystem/JGMaterial/JG2DMesh.h"
#include"EngineStatics/JGConstructHelper.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"Character/Player.h"
#include"EngineStatics/JGLog.h"
using namespace std;
Anim_Player::Anim_Player()
{
	Idle = AddAnimation(EAnimState::Anim_Idle, TT("Anim_Player_Idle"));
	if (Idle)
	{
		static JGConstructHelper::AnimationMesh2D Anim_Idle_Mesh(
			Idle->GetComponentName(), EPivot::MiddleMiddle, 8, 4, 2,
			TT("../Contents/Player/Idle/AnimSheet_Player_Idle.png"),
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
			TT("../Contents/Player/Move/AnimSheet_Player_Move.png"),EReverse::Default,
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
	DefaultAttack_1 = AddAnimation(EAnimState::Anim_DefaultAttack_1, TT("Anim_Player_DefaultAttack_1"));
	if (DefaultAttack_1)
	{
		static JGConstructHelper::AnimationMesh2D Anim_DefaultAttack_1_Mesh(
			DefaultAttack_1->GetComponentName(), EPivot::MiddleMiddle, 6, 3, 2,
			TT("../Contents/Player/DefaultAttack/AnimSheet_Player_DefaultAttack_1.png"),
			EReverse::Default, EJGUsageType::Dynamic);
		if (Anim_DefaultAttack_1_Mesh.Success)
		{
			DefaultAttack_1->SetConstructObject(Anim_DefaultAttack_1_Mesh.Object);
		}
		DefaultAttack_1->AnimationSetDelay(0.05f);
	}
	DefaultAttack_2 = AddAnimation(EAnimState::Anim_DefaultAttack_2, TT("Anim_Player_DefaultAttack_2"));
	if (DefaultAttack_2)
	{
		static JGConstructHelper::AnimationMesh2D Anim_DefaultAttack_2_Mesh(
			DefaultAttack_2->GetComponentName(), EPivot::MiddleMiddle, 5, 3, 2,
			TT("../Contents/Player/DefaultAttack/AnimSheet_Player_DefaultAttack_2.png"),
			EReverse::Default, EJGUsageType::Dynamic);
		if (Anim_DefaultAttack_2_Mesh.Success)
		{
			DefaultAttack_2->SetConstructObject(Anim_DefaultAttack_2_Mesh.Object);
		}
		DefaultAttack_2->AnimationSetDelay(0.05f);
	}


	SetCurrentState(Anim_Idle);
}

Anim_Player::~Anim_Player()
{


}

void Anim_Player::BeginComponent(World* world)
{
	Animation2DSystemComponent::BeginComponent(world);

	AddNotifyEvent(TT("Stop_by_Jump_FallingUp"), Anim_Jump, 3, [this]()
	{
		this->GetAnimation(Anim_Jump)->Stop();
	});
	AddNotifyEvent(TT("Stop_by_Jump_FallingDown"), Anim_Jump, 7, [this]()
	{
		this->GetAnimation(Anim_Jump)->Stop();
	});
	AddNotifyEvent(TT("Stop_by_SitDown"), Anim_SitDown, 4, [this]()
	{
		this->GetAnimation(Anim_SitDown)->Stop();
	});
	AddNotifyEvent(TT("Change_Stand_Up_To_Idle"), Anim_StandUp, 4, [this]()
	{
		Player* p = dynamic_cast<Player*>(this->GetOwnerObject());
		if (p)
		{
			p->SetCurrentPlayerState(Player_Idle);
		}
		this->GetAnimation(Anim_StandUp)->Stop();
	});
}

void Anim_Player::Tick(const float DeltaTime)
{
	Animation2DSystemComponent::Tick(DeltaTime);


	Player* p = dynamic_cast<Player*>(GetOwnerObject());
	if (p)
	{
		if (PrevPlayerState != p->GetCurrentPlayerState() || p->IsWorking())
		{
			switch (p->GetCurrentPlayerState())
			{
			case EPlayerState::Player_Idle:
				ConfigDefault(p,Anim_Idle);
				break;
			case EPlayerState::Player_Move:
				ConfigDefault(p,Anim_Move);
				break;
			case EPlayerState::Player_SitDown:
				ConfigDefault(p, Anim_SitDown);
				break;
			case EPlayerState::Player_StandUp:
				ConfigDefault(p, Anim_StandUp);
				break;
			case EPlayerState::Player_JumpUp:
			case EPlayerState::Player_JumpDown:
				ConfigJump(p);
				break;
			}
		}
		PrevPlayerState = p->GetCurrentPlayerState();
	}
	


}
void Anim_Player::ConfigDefault(Player * p, EAnimState state)
{
	if (p->IsRight())
	{
		GetAnimation(state)->Reverse(EReverse::Default);
	}
	else
	{
		GetAnimation(state)->Reverse(EReverse::RL);
	}
	SetCurrentState(state);
}
void Anim_Player::ConfigJump(Player * p)
{
	if (p->GetCurrentPlayerState() == Player_JumpDown)
	{
		if (p->IsRight())
		{
			GetAnimation(Anim_Jump)->Reverse(EReverse::Default);
		}
		else
		{
			GetAnimation(Anim_Jump)->Reverse(EReverse::RL);
		}
		SetCurrentState(Anim_Jump);
		GetAnimation(Anim_Jump)->SetCurrentFrame(4);
		GetAnimation(Anim_Jump)->Play();
	}
	else if (p->GetCurrentPlayerState() == Player_JumpUp)
	{
		if (p->IsRight())
		{
			GetAnimation(Anim_Jump)->Reverse(EReverse::Default);
		}
		else
		{
			GetAnimation(Anim_Jump)->Reverse(EReverse::RL);
		}
		SetCurrentState(Anim_Jump);
		if (GetAnimation(Anim_Jump)->AnimationIsFrame(3))
		{
			GetAnimation(Anim_Jump)->Stop();
		}
	}
}

void Anim_Player::ConfigAttack(Player* p)
{

}
