#pragma once
#include"EngineFrameWork/Object/Side_Scroll_Unit.h"

class EnemyUnitBase : public Side_Scroll_Unit
{
private: 
	class BT_EnemyAIBase* BT_EnemyAI = nullptr;
	class EnemyStatus* Status = nullptr;
public:
	EnemyUnitBase();
	virtual ~EnemyUnitBase();
	virtual void Tick(const float DeltaTime) override;
	virtual void ReceiveSingleDamageProcess(class SingleDamage* dmg) override;
};