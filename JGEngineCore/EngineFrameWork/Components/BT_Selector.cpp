#include"BT_Selector.h"

BT_Selector::BT_Selector()
{
	RegisterComponentID(typeid(this));
	m_NodeType = EBTNodeType::Selector;
}

BT_Selector::~BT_Selector()
{
}

void BT_Selector::Tick(const float DeltaTime)
{
}

BT_Selector::EBTNodeType BT_Selector::GetNodeType()
{
	return m_NodeType;
}
