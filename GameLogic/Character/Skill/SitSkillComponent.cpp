#include"AttackBaseComponent.h"
#include "SitSkillComponent.h"

SitSkillComponent::SitSkillComponent()
{
	RegisterComponentID(typeid(this));
	SetAttackDelay(10.0f);
}

SitSkillComponent::~SitSkillComponent()
{
}
