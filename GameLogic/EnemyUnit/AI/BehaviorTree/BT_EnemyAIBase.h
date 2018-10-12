#pragma once
#include"EngineFrameWork/Components/BehaviorTreeComponent.h"



class BT_EnemyAIBase : public BehaviorTreeComponent
{
private:
	class BTIB_EnemyBoard* EnemyBoard;

public:
	BT_EnemyAIBase();
	virtual ~BT_EnemyAIBase();
	virtual void MakeAITreeSpace() override;
};