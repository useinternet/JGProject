#include"DefaultAttackComponent.h"
#include"EngineStatics/JTimerManager.h"
using namespace std;
DefaultAttackComponent::DefaultAttackComponent()
{
	RegisterComponentID(typeid(this));
	SetAttackDelay(0.5f);
}

DefaultAttackComponent::~DefaultAttackComponent()
{

}


