#include "BT_EnemyAIBase.h"
#include"EnemyUnit/AI/Board/BTIB_EnemyBoard.h"
BT_EnemyAIBase::BT_EnemyAIBase()
{
	RegisterComponentID(typeid(this));
}

BT_EnemyAIBase::~BT_EnemyAIBase()
{

}

void BT_EnemyAIBase::MakeAITreeSpace()
{
	BehaviorTreeComponent::MakeAITreeSpace();

	

	EnemyBoard = CreateBoard<BTIB_EnemyBoard>(TT("dd"));
	EnemyBoard->EnemyObject = GetOwnerObject();


}
