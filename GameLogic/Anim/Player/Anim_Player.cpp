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
		DefaultAttack_1->AnimationSetDelay(0.075f);
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
		DefaultAttack_2->AnimationSetDelay(0.075f);
	}
	DefaultSkill = AddAnimation(EAnimState::Anim_DefaultSkill, TT("Anim_Player_DefaultSkill"));
	if (DefaultSkill)
	{
		static JGConstructHelper::AnimationMesh2D Anim_DefaultSkill_Mesh(
			DefaultSkill->GetComponentName(), EPivot::MiddleMiddle, 6, 3, 2,
			TT("../Contents/Player/DefaultSkill/AnimSheet_Player_DefaultSkill.png"),
			EReverse::Default, EJGUsageType::Dynamic);
		if (Anim_DefaultSkill_Mesh.Success)
		{
			DefaultSkill->SetConstructObject(Anim_DefaultSkill_Mesh.Object);
		}
		DefaultSkill->AnimationSetDelay(0.1f);
	}
	SpecialSkill = AddAnimation(EAnimState::Anim_SpecialSkill, TT("Anim_Player_SpecialSkill"));
	if (SpecialSkill)
	{
		static JGConstructHelper::AnimationMesh2D Anim_SpecialSkill_Mesh(
			SpecialSkill->GetComponentName(), EPivot::MiddleMiddle, 11, 4, 3,
			TT("../Contents/Player/SpecialSkill/AnimSheet_Player_SpecialSkill.png"),
			EReverse::Default, EJGUsageType::Dynamic);
		if (Anim_SpecialSkill_Mesh.Success)
		{
			SpecialSkill->SetConstructObject(Anim_SpecialSkill_Mesh.Object);
		}
		SpecialSkill->AnimationSetDelay(0.1f);
	}
	JumpDefaultAttack = AddAnimation(EAnimState::Anim_JumpDefaultAttack,TT("Anim_Player_JumpDefaultAttack_1"));
	if (JumpDefaultAttack)
	{
		static JGConstructHelper::AnimationMesh2D Anim_JumpDefaultAttack_Mesh(
			JumpDefaultAttack->GetComponentName(), EPivot::MiddleMiddle, 4, 4, 1,
			TT("../Contents/Player/JumpDefaultAttack/AnimSheet_Player_JumpDefaultAttack_1.png"),
			EReverse::Default,EJGUsageType::Dynamic);
		if (Anim_JumpDefaultAttack_Mesh.Success)
		{
			JumpDefaultAttack->SetConstructObject(Anim_JumpDefaultAttack_Mesh.Object);
		}
		JumpDefaultAttack->AnimationSetDelay(0.1f);
	}
	SitDefaultAttack_1 = AddAnimation(EAnimState::Anim_SitDefaultAttack_1, TT("Anim_Player_SitDefaultAttack_1"));
	if (SitDefaultAttack_1)
	{
		static JGConstructHelper::AnimationMesh2D Anim_SitDefaultAttack_1_Mesh(
			SitDefaultAttack_1->GetComponentName(), EPivot::MiddleMiddle, 5, 3, 2,
			TT("../Contents/Player/SitDefaultAttack/AnimSheet_Player_SitDefaultAttack_1.png"),
			EReverse::Default, EJGUsageType::Dynamic);
		if (Anim_SitDefaultAttack_1_Mesh.Success)
		{
			SitDefaultAttack_1->SetConstructObject(Anim_SitDefaultAttack_1_Mesh.Object);
		}
		SitDefaultAttack_1->AnimationSetDelay(0.1f);
	}
	SitDefaultAttack_2 = AddAnimation(EAnimState::Anim_SitDefaultAttack_2, TT("Anim_Player_SitDefaultAttack_2"));
	if (SitDefaultAttack_2)
	{
		static JGConstructHelper::AnimationMesh2D Anim_SitDefaultAttack_2_Mesh(
			SitDefaultAttack_2->GetComponentName(), EPivot::MiddleMiddle, 4, 4, 1,
			TT("../Contents/Player/SitDefaultAttack/AnimSheet_Player_SitDefaultAttack_2.png"),
			EReverse::Default, EJGUsageType::Dynamic);
		if (Anim_SitDefaultAttack_2_Mesh.Success)
		{
			SitDefaultAttack_2->SetConstructObject(Anim_SitDefaultAttack_2_Mesh.Object);
		}
		SitDefaultAttack_2->AnimationSetDelay(0.1f);
	}
	SitDownSkill = AddAnimation(EAnimState::Anim_SitDownSkill, TT("Anim_Player_SitDownSkill"));
	if (SitDownSkill)
	{
		static JGConstructHelper::AnimationMesh2D Anim_SitDownSkill_Mesh(
			SitDownSkill->GetComponentName(), EPivot::MiddleMiddle, 7, 4, 2,
			TT("../Contents/Player/SitSkill/AnimSheet_Player_SitSkill.png"),
			EReverse::Default, EJGUsageType::Dynamic);
		if (Anim_SitDownSkill_Mesh.Success)
		{
			SitDownSkill->SetConstructObject(Anim_SitDownSkill_Mesh.Object);
		}
		SitDownSkill->AnimationSetDelay(0.1f);
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
	AddNotifyEvent(TT("DefaultAttack_1_To_Idle"), Anim_DefaultAttack_1, 6, [this]()
	{
		AttackChangeIdleFromEnd();
		this->GetAnimation(Anim_DefaultAttack_1)->Stop();
	});
	AddNotifyEvent(TT("DefaultAttack_2_To_Idle"), Anim_DefaultAttack_2, 5, [this]()
	{
		AttackChangeIdleFromEnd();
		this->GetAnimation(Anim_DefaultAttack_2)->Stop();
	});
	AddNotifyEvent(TT("DefaultSkill_To_Idle"), Anim_DefaultSkill, 6, [this]()
	{
		AttackChangeIdleFromEnd();
		this->GetAnimation(Anim_DefaultSkill)->Stop();
	});
	AddNotifyEvent(TT("SpecialSkill_To_Idle"), Anim_SpecialSkill, 11, [this]()
	{
		AttackChangeIdleFromEnd();
		this->GetAnimation(Anim_SpecialSkill)->Stop();
	});
	AddNotifyEvent(TT("SitDefaultAttack_1_To_SitDown"), Anim_SitDefaultAttack_1, 5, [this]()
	{
		SitAttackChangeSitDownFromEnd();
		this->GetAnimation(Anim_SitDefaultAttack_1)->Stop();
	});
	AddNotifyEvent(TT("SitDefaultAttack_2_To_SitDown"), Anim_SitDefaultAttack_2, 4, [this]()
	{
		SitAttackChangeSitDownFromEnd();
		this->GetAnimation(Anim_SitDefaultAttack_2)->Stop();
	});
	AddNotifyEvent(TT("SitDownSkill_To_SitDown"),Anim_SitDownSkill,7,[this]()
	{
		SitAttackChangeSitDownFromEnd();
		this->GetAnimation(Anim_SitDownSkill)->Stop();
	});
	AddNotifyEvent(TT("JumpAttack_To_JumpDown"), Anim_JumpDefaultAttack, 4, [this]()
	{
		JumpAttackChangeJumpDownFromEnd();
		this->GetAnimation(Anim_JumpDefaultAttack)->Stop();
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
				ConfigSitDown(p);
				break;
			case EPlayerState::Player_StandUp:
				ConfigDefault(p, Anim_StandUp);
				break;
			case EPlayerState::Player_DefaultAttack:
				ConfigAttack(p);
				break;
			case EPlayerState::Player_SitAttack:
				ConfigSitAttack(p);
				break;
			case EPlayerState::Player_SitSkill:
				ConfigDefault(p, Anim_SitDownSkill);
				break;
			case EPlayerState::Player_DefaultSkill:
				ConfigDefault(p, Anim_DefaultSkill);
				break;
			case EPlayerState::Player_SpecialSkill:
				ConfigDefault(p, Anim_SpecialSkill);
				break;
			case EPlayerState::Player_JumpUp:
			case EPlayerState::Player_JumpDown:
				ConfigJump(p);
				break;
			case EPlayerState::Player_JumpAttack:
				ConfigDefault(p, Anim_JumpDefaultAttack);
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
void Anim_Player::ConfigSitDown(Player* p)
{
	ConfigDefault(p, Anim_SitDown);
	if (p->IsCompulsoryDirection())
	{
		GetAnimation(Anim_SitDown)->SetCurrentFrame(4);
		GetAnimation(Anim_SitDown)->Stop();
		p->NotifySolveCompulsoryDirection();
	}
}
void Anim_Player::ConfigJump(Player* p)
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
	int num = JGMath::RandomDraw(0, 1);
	if (num == 0)
	{
		ConfigDefault(p, Anim_DefaultAttack_1);
	}
	else
	{
		ConfigDefault(p, Anim_DefaultAttack_2);
	}
}

void Anim_Player::ConfigSitAttack(Player * p)
{
	int num = JGMath::RandomDraw(0, 1);
	if (num == 0)
	{
		ConfigDefault(p, Anim_SitDefaultAttack_1);
	}
	else
	{
		ConfigDefault(p, Anim_SitDefaultAttack_2);
	}
}
void Anim_Player::AttackChangeIdleFromEnd()
{
	Player* p = dynamic_cast<Player*>(this->GetOwnerObject());
	if (p)
	{
		p->NotifyAttackComplete();
		p->SetCurrentPlayerState(Player_Idle);
	}
}

void Anim_Player::SitAttackChangeSitDownFromEnd()
{
	Player* p = dynamic_cast<Player*>(this->GetOwnerObject());
	if (p)
	{
		p->NotifyAttackComplete();
		p->SetCurrentPlayerState(Player_SitDown);
	}
}

void Anim_Player::JumpAttackChangeJumpDownFromEnd()
{
	Player* p = dynamic_cast<Player*>(this->GetOwnerObject());
	if (p)
	{
		p->NotifyAttackComplete();
		p->SetCurrentPlayerState(Player_JumpDown);
	}
}
