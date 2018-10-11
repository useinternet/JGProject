#include"BT_Task.h"

BT_Task::BT_Task()
{
	m_NodeType = EBTNodeType::Task;

}

BT_Task::~BT_Task()
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
BT_Task::EBTNodeType BT_Task::GetNodeType()
{
	return m_NodeType;
}
