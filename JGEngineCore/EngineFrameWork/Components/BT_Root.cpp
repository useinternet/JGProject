#include"BT_Root.h"

BT_Root::BT_Root()
{
	RegisterComponentID(typeid(this));
	m_NodeType = EBTNodeType::Root;
}

BT_Root::~BT_Root()
{
}

void BT_Root::Tick(const float DeltaTime)
{
}

BT_Root::EBTNodeType BT_Root::GetNodeType()
{
	return m_NodeType;
}
