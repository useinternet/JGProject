#include"BT_Root.h"
using namespace std;
BT_Root::BT_Root()
{
	m_NodeType = EBTNodeType::Root;
}

BT_Root::~BT_Root()
{
}

void BT_Root::Behavior(const float DeltaTime)
{
	BT_MiddleNode::Behavior(DeltaTime);
	if (GetChildNodeList_p()->empty() || IsPass())
	{
		return;
	}
	auto ExcuteNode = GetChildNodeList_p()->begin();
	advance(ExcuteNode, m_CurrentCount);

	(*ExcuteNode)->Behavior(DeltaTime);
	

	if ((*ExcuteNode)->IsPass())
	{
		m_CurrentCount++;
	}
	if (m_CurrentCount >= GetListSize())
	{
		Pass();
		m_CurrentCount = 0;
	}
}


BT_Root::EBTNodeType BT_Root::GetNodeType()
{
	return m_NodeType;
}
