#include"BT_Task.h"
#include"BT_InformationBoard.h"
BT_Task::BT_Task()
{
	m_NodeType = EBTNodeType::Task;

}

BT_Task::~BT_Task()
{

}
void BT_Task::BeginTask()
{

}
void BT_Task::Behavior(const float DeltaTime)
{
	NoPass();
	bool result = ExcuteTask(DeltaTime);
	result ? Pass() : NoPass();
}
bool BT_Task::ExcuteTask(const float DeltaTime)
{
	return true;
}
EBTNodeType BT_Task::GetNodeType()
{
	return m_NodeType;
}
void BT_Task::SetBoard(BT_InformationBoard* board)
{
	m_Board = board;
}

BT_InformationBoard* BT_Task::GetBoard()
{
	return m_Board;
}
