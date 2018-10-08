#include"AttackBaseComponent.h"
#include"EngineStatics/JTimerManager.h"
#include"EngineStatics/JGLog.h"
#include"DefaultSkillComponent.h"
using namespace std;
AttackBaseComponent::AttackBaseComponent()
{
	RegisterComponentID(typeid(this));
}

AttackBaseComponent::~AttackBaseComponent()
{

}

void AttackBaseComponent::SetAttackDelay(const float Delay)
{
	if (AttackTimerHandle)
	{
		AttackTimerHandle->Destory();
	}
	AttackDelay = Delay;
	JTimerEventManager::CreateTimerEvent(&AttackTimerHandle,
		bind(&AttackBaseComponent::AttackTimerEvent, this), EHandleType::ERepeat,
		0.0f, AttackDelay, -1);
}
float AttackBaseComponent::GetAttackDelay()
{
	return AttackDelay;
}
bool AttackBaseComponent::IsEnableAttack()
{
	return bEnableAttack;
}
void AttackBaseComponent::Attack(const JGVector2D& Location)
{
	bEnableAttack = false;
}
void AttackBaseComponent::Attack(const float x, const float y)
{
	Attack(JGVector2D(x, y));
}
void AttackBaseComponent::AttackTimerEvent()
{
	bEnableAttack = true;
}
