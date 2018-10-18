#pragma once
#include"EngineFrameWork/Components/Component.h"

class JTimerHandle;
class AttackBaseComponent : public Component
{
private:
	float AttackDelay = 0.5f;
	bool  bEnableAttack = false;
	JTimerHandle* AttackTimerHandle = nullptr;

public:
	AttackBaseComponent();
	virtual ~AttackBaseComponent();
	void SetAttackDelay(const float Delay);
	float GetAttackDelay();
	bool IsEnableAttack();
	virtual void Attack(const JGVector2D& Location, const float VelScale = 1.0f);
	virtual void Attack(const float x, const float y, const float VelScale = 1.0f);
private:
	void AttackTimerEvent();
};