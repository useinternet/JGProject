#include"BT_Selector.h"
using namespace std;
BT_Selector::BT_Selector()
{
	m_NodeType = EBTNodeType::Selector;
}

BT_Selector::~BT_Selector()
{

}
void BT_Selector::InitNode()
{
	BT_MiddleNode::InitNode();
	m_CurrentCount = 0;
}
void BT_Selector::Behavior(const float DeltaTime)
{
	BT_MiddleNode::Behavior(DeltaTime);
	if (GetChildNodeList_p()->empty())
	{
		Pass();
		return;
	}
	NoPass();
	auto ExcuteNode = GetChildNodeList_p()->begin();
	advance(ExcuteNode, m_CurrentCount);

	// 노드 행동
	(*ExcuteNode)->Behavior(DeltaTime);
	// 해당 노드가 true 일경우에는
	if ((*ExcuteNode)->IsPass())
	{
		InitNode();
		Pass(); // 패스 = true
	}
	// 해당 노드가 false일 경우에는
	else
	{
		// 다음에는 다음 노드를 실행 시키며.. 
		m_CurrentCount++;
		// 현재 위치의 노드가 리스트의 사이즈를 벗어낫을경우 다시 처음부터 노드를 실행..
		if (m_CurrentCount >= GetListSize()) 
		{
			m_CurrentCount = 0;
		}
		// Pass = false;
		NoPass();
	}
}
EBTNodeType BT_Selector::GetNodeType()
{
	return m_NodeType;
}
