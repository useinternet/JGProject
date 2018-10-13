#include"BT_Sequence.h"

BT_Sequence::BT_Sequence()
{
	m_NodeType = EBTNodeType::Sequence;
}

BT_Sequence::~BT_Sequence()
{
}

void BT_Sequence::InitNode()
{
	BT_MiddleNode::InitNode();
	m_CurrentCount = 0;
	m_bChildNodeAllPass = true;
}

void BT_Sequence::Behavior(const float DeltaTime)
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
	// 해당 위치의 노드 행동 개시..
	(*ExcuteNode)->Behavior(DeltaTime);

	// 실행 노드가 false를 반환했다면..
	// 모든 자식노드가 true를 반환 하지 않았으므로 일단 체크해둔다.
	if (!(*ExcuteNode)->IsPass())
	{
		m_bChildNodeAllPass = false;
	}
	//  현재위치를 다음위치로 옮기고..
	m_CurrentCount++;

	// 사이즈 체크
	if (m_CurrentCount >= GetListSize())
	{
		// 자식노드가 모두 true를 반환했으면 pass 아니면 nopass
		m_bChildNodeAllPass ? Pass() : NoPass();
		InitNode();
	}
}

EBTNodeType BT_Sequence::GetNodeType()
{
	return m_NodeType;
}
