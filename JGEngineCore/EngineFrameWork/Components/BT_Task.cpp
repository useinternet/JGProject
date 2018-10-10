#include"BT_Task.h"

BT_Task::BT_Task()
{
	RegisterComponentID(typeid(this));
	m_NodeType = EBTNodeType::Task;

}

BT_Task::~BT_Task()
{
}

void BT_Task::Tick(const float DeltaTime)
{

}

BT_Task::EBTNodeType BT_Task::GetNodeType()
{
	return m_NodeType;
}
