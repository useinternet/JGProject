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
	void Attack();
private:
	void AttackTimerEvent();
};