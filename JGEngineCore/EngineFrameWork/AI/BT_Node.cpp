#include"BT_Node.h"
#include"BT_MiddleNode.h"
#include"BT_InformationBoard.h"

using namespace std;
BT_Node::BT_Node()
{

}

BT_Node::~BT_Node()
{

}
void BT_Node::InitNode()
{

}
void BT_Node::Behavior(const float DeltaTime)
{

}

void BT_Node::RegisterName(const wstring& name)
{
	m_NodeName = name;
}


bool BT_Node::IsPass()
{
	return m_IsPass;
}

void BT_Node::Pass()
{
	m_IsPass = true;
}

void BT_Node::NoPass()
{
	m_IsPass = false;
}

BT_MiddleNode* BT_Node::GetParentNode()
{
	return m_ParentNode;
}
void BT_Node::ThisIsMyParent(BT_MiddleNode* node)
{
	m_ParentNode = node;
}

BT_Node::EBTNodeType BT_Node::GetNodeType()
{
	return EBTNodeType(EBTNodeType::None);
}

const wstring& BT_Node::GetNodeName()
{
	return m_NodeName;
}
