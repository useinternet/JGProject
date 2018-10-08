#pragma once
#include"AttackBaseComponent.h"

class JTimerHandle;
class DefaultAttackComponent : public AttackBaseComponent
{
public:
	DefaultAttackComponent();
	virtual ~DefaultAttackComponent();

	virtual void Attack(const JGVector2D& Location) override;
	virtual void Attack(const float x, const float y) override;
};