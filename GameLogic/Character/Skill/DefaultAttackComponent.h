#pragma once
#include"AttackBaseComponent.h"

class JTimerHandle;
class DefaultAttackComponent : public AttackBaseComponent
{
public:
	DefaultAttackComponent();
	virtual ~DefaultAttackComponent();

	virtual void Attack(const JGVector2D& Location, const float VelScale = 1.0f) override;
	virtual void Attack(const float x, const float y, const float VelScale = 1.0f) override;
};