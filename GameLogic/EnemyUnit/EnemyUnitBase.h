#pragma once
#include"EngineFrameWork/Object/Side_Scroll_Unit.h"

class EnemyUnitBase : public Side_Scroll_Unit
{
private: 
	class BT_EnemyAIBase* BT_EnemyAI = nullptr;
public:
	EnemyUnitBase();
	virtual ~EnemyUnitBase();

	virtual void ReceiveSingleDamageProcess(class SingleDamage* dmg) override;
};