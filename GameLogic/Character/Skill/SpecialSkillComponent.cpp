#include"SpecialSkillComponent.h"

SpecialSkillComponent::SpecialSkillComponent()
{
	RegisterComponentID(typeid(this));
	SetAttackDelay(10.0f);
}

SpecialSkillComponent::~SpecialSkillComponent()
{
}
