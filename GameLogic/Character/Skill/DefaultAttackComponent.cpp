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

void DefaultAttackComponent::Attack(const JGVector2D& Location)
{
	AttackBaseComponent::Attack(Location);
	Object* obj = GetWorld()->SpawnObject<DefaultAttackObject>();

	DefaultAttackObject* AttackObject = dynamic_cast<DefaultAttackObject*>(obj);
	if (AttackObject)
	{
		AttackObject->SetAttackSpawnLocation(Location);
		JGLog::Write(ELogLevel::Default, TT("SetAttackSpawnLocation"));
	}

}

void DefaultAttackComponent::Attack(const float x, const float y)
{
	AttackBaseComponent::Attack(x,y);
	Object* obj = GetWorld()->SpawnObject<DefaultAttackObject>();

	DefaultAttackObject* AttackObject = dynamic_cast<DefaultAttackObject*>(obj);
	if (AttackObject)
	{
		AttackObject->SetAttackSpawnLocation(x,y);
		JGLog::Write(ELogLevel::Default, TT("SetAttackSpawnLocation"));
	}
}


