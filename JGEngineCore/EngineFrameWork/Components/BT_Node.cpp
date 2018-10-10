#include"BT_Node.h"

BT_Node::BT_Node()
{
	RegisterComponentID(typeid(this));
}

BT_Node::~BT_Node()
{
}

BT_Node::EBTNodeType BT_Node::GetNodeType()
{
	return EBTNodeType(EBTNodeType::None);
}
