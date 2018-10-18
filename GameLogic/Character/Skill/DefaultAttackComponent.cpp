#include"DefaultAttackComponent.h"
#include"EngineStatics/JTimerManager.h"
#include"Character/SkillObject/DefaultAttackObject.h"
#include"EngineFrameWork/Object/Unit.h"
#include"EngineFrameWork/World/World.h"
#include"EngineStatics/JGLog.h"
using namespace std;
DefaultAttackComponent::DefaultAttackComponent()
{
	RegisterComponentID(typeid(this));
	SetAttackDelay(0.5f);
}

DefaultAttackComponent::~DefaultAttackComponent()
{

}

void DefaultAttackComponent::Attack(const JGVector2D& Location, const float VelScale)
{
	AttackBaseComponent::Attack(Location,VelScale);
	Object* obj = GetWorld()->SpawnObject<DefaultAttackObject>();

	DefaultAttackObject* AttackObject = dynamic_cast<DefaultAttackObject*>(obj);
	if (AttackObject)
	{
		AttackObject->SetAttackSpawnLocation(Location,VelScale);
	}

}

void DefaultAttackComponent::Attack(const float x, const float y, const float VelScale)
{
	AttackBaseComponent::Attack(x,y,VelScale);
	Object* obj = GetWorld()->SpawnObject<DefaultAttackObject>();

	DefaultAttackObject* AttackObject = dynamic_cast<DefaultAttackObject*>(obj);
	if (AttackObject)
	{
		AttackObject->SetAttackSpawnLocation(x,y, VelScale);
	}
}


