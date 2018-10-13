#pragma once
#include"EngineFrameWork/Components/BehaviorTreeComponent.h"



class BT_EnemyAIBase : public BehaviorTreeComponent
{
private:
	class BTT_MoveTo* MoveToTask = nullptr;
public:
	BT_EnemyAIBase();
	virtual ~BT_EnemyAIBase();
	virtual void InformationBoardInitSpace() override;
	virtual void MakeAITreeSpace() override;
};