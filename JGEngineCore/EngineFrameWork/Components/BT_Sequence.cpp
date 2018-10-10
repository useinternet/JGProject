#include"BT_Sequence.h"

BT_Sequence::BT_Sequence()
{
	RegisterComponentID(typeid(this));
	m_NodeType = EBTNodeType::Sequence;
}

BT_Sequence::~BT_Sequence()
{
}

void BT_Sequence::Tick(const float DeltaTime)
{
}

BT_Sequence::EBTNodeType BT_Sequence::GetNodeType()
{
	return m_NodeType;
}
