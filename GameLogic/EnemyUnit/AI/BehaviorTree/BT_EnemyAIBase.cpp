#include "BT_EnemyAIBase.h"
#include"EnemyUnit/AI/Board/BTIB_EnemyBoard.h"
#include"EnemyUnit/AI/Task/BTT_MoveTo.h"
BT_EnemyAIBase::BT_EnemyAIBase()
{
	RegisterComponentID(typeid(this));
	// 여기에는 초기화만..


	MoveToTask = CreateTask<BTT_MoveTo>(TT("BTT_MoveTo"));

}
BT_EnemyAIBase::~BT_EnemyAIBase()
{

}
void BT_EnemyAIBase::InformationBoardInitSpace()
{
	BehaviorTreeComponent::InformationBoardInitSpace();
}
void BT_EnemyAIBase::MakeAITreeSpace()
{
	BehaviorTreeComponent::MakeAITreeSpace();
}
