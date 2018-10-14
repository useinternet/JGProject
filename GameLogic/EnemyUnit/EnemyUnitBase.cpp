#include "EnemyUnitBase.h"
#include"EngineFrameWork/Components/Box2DCollisionComponent.h"
#include"StaticFilter/StaticCollisionFilter.h"
#include"EnemyStatus.h"
#include"AI/BehaviorTree/BT_EnemyAIBase.h"
#include"EngineFrameWork/DamageInformation/SingleDamage.h"
#include"Character/Player.h"
#include"EngineStatics/JGLog.h"
EnemyUnitBase::EnemyUnitBase()
{
	RegisterObjectID(typeid(this));
	GetCollision()->SetCategoryFilter(Filter_Enemy);
	GetCollision()->SetMaskFilter(Filter_Ground | Filter_PlayerAttack | Filter_PlayerCollision);
	GetCollision()->SetAsBox(50.0f, 200.0f);
	GetCollision()->SetComponentLocation(1400.0f, 800.0f);

	Status = RegisterComponentInObject<EnemyStatus>(TT("EnemyStatus"));
	Status->SetMaxHp(100.0f);


	BT_EnemyAI = RegisterComponentInObject<BT_EnemyAIBase>(TT("BT_EnemyAI"));
}

EnemyUnitBase::~EnemyUnitBase()
{

}
void EnemyUnitBase::Tick(const float DeltaTime)
{
	Side_Scroll_Unit::Tick(DeltaTime);

	if (Status->GetCurrentHp() <= 0)
	{
		GetCollision()->DestroyCollison();
		DestoryObject();
	}
}

void EnemyUnitBase::ReceiveSingleDamageProcess(SingleDamage* dmg)
{
	if (dmg == nullptr) return;

	Status->AddCurrentHp(-(dmg->Damage));
	JGLog::Write(ELogLevel::Error, TT("%d"), (int)dmg->Damage);
	JGLog::Write(ELogLevel::Error, TT("%d"), (int)Status->GetCurrentHp());
}
