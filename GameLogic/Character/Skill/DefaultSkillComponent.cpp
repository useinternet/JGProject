#include"DefaultSkillComponent.h"

DefaultSkillComponent::DefaultSkillComponent()
{
	RegisterComponentID(typeid(this));
	SetAttackDelay(10.0f);
}

DefaultSkillComponent::~DefaultSkillComponent()
{
}
