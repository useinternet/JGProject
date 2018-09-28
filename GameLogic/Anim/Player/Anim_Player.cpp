#include"Anim_Player.h"
#include"EngineFrameWork/Components/AnimationMesh2DComponent.h"
#include"RenderSystem/JGMaterial/JG2DMesh.h"
#include"EngineStatics/JGConstructHelper.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"Character/Player.h"


using namespace std;
Anim_Player::Anim_Player()
{
	IdleStart = AddAnimation(EAnimState::Anim_IdleStart, TT("Anim_Player_StartIdle"));
	if (IdleStart)
	{
		static JGConstructHelper::AnimationMesh2D Anim_IdleStart_Mesh(
			IdleStart->GetComponentName(), EPivot::MiddleMiddle, 9, 3, 3,
			TT("../GameLogic/Contents/Player/Idle1-1/AnimSheet_Player_Idle 1-1.png"));
		if (Anim_IdleStart_Mesh.Success)
		{
			IdleStart->SetConstructObject(Anim_IdleStart_Mesh.Object);
		}
	}
	IdleStart->AnimationSetDelay(0.1f);
	Idle = AddAnimation(EAnimState::Anim_Idle, TT("Anim_Player_Idle"));
	if (Idle)
	{
		static JGConstructHelper::AnimationMesh2D Anim_Idle_Mesh(
			Idle->GetComponentName(), EPivot::MiddleMiddle, 8, 4, 2,
			TT("../GameLogic/Contents/Player/Idle1-2/AnimSheet_Idle 1-2.png"));
		if (Anim_Idle_Mesh.Success)
		{
			Idle->SetConstructObject(Anim_Idle_Mesh.Object);
		}
	}
	Idle->AnimationSetDelay(0.1f);
	RightMove = AddAnimation(EAnimState::Anim_RightMove, TT("Anim_Player_RightMove"));
	if (RightMove)
	{
		static JGConstructHelper::AnimationMesh2D Anim_RightMove_Mesh(
			RightMove->GetComponentName(), EPivot::MiddleMiddle, 6, 3, 2,
			TT("../GameLogic/Contents/Player/Move/AnimSheet_Move.png"));
		if (Anim_RightMove_Mesh.Success)
		{
			RightMove->SetConstructObject(Anim_RightMove_Mesh.Object);
		}
	}
	RightMove->AnimationSetDelay(0.1f);
	LeftMove = AddAnimation(EAnimState::Anim_LeftMove, TT("Anim_Player_LeftMove"));
	if (LeftMove)
	{
		static JGConstructHelper::AnimationMesh2D Anim_LeftMove_Mesh(
			LeftMove->GetComponentName(), EPivot::MiddleMiddle,6,3,2,
			TT("../GameLogic/Contents/Player/Move/AnimSheet_Move.png"),
			EReverse::RL);
		if (Anim_LeftMove_Mesh.Success)
		{
			LeftMove->SetConstructObject(Anim_LeftMove_Mesh.Object);
		}
	}
	LeftMove->AnimationSetDelay(0.1f);
	Jump = AddAnimation(EAnimState::Anim_Jump, TT("Anim_Player_Jump"));
	if (Jump)
	{
		static JGConstructHelper::AnimationMesh2D Anim_Jump_Mesh(
			Jump->GetComponentName(), EPivot::MiddleMiddle, 7, 4, 2,
			TT("../GameLogic/Contents/Player/Jump/AnimSheet_Player_Jump.png"));
		if (Anim_Jump_Mesh.Success)
		{
			Jump->SetConstructObject(Anim_Jump_Mesh.Object);
		}
	}
	Jump->AnimationSetDelay(0.1f);
	SetCurrentState(Anim_IdleStart);

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
		if (p->GetCurrentPlayerState() == Player_JumpDown && !p->IsFalling())
		{
			p->SetCurrentPlayerState(Player_Idle);
		}
		if (PrevPlayerState != p->GetCurrentPlayerState())
		{
			switch (p->GetCurrentPlayerState())
			{
			case EPlayerState::Player_Idle:
				SetCurrentState(Anim_Idle);
				break;
			case EPlayerState::Player_JumpUp:
				SetCurrentState(Anim_Jump);
				break;
			case EPlayerState::Player_JumpDown:
				SetCurrentState(Anim_Jump);
				GetAnimation(Anim_Jump)->SetCurrentFrame(4);
				GetAnimation(Anim_Jump)->Play();
				break;
			case EPlayerState::Player_LeftMove:
				SetCurrentState(Anim_LeftMove);
				break;
			case EPlayerState::Player_RightMove:
				SetCurrentState(Anim_RightMove);
				break;
			case EPlayerState::Player_Sit:
				break;
			}
		}





		PrevPlayerState = p->GetCurrentPlayerState();
	}
	


}