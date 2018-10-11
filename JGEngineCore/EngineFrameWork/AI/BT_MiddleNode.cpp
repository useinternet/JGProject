#include"BT_MiddleNode.h"
using namespace std;


BT_MiddleNode::BT_MiddleNode()
{

}
BT_MiddleNode::~BT_MiddleNode()
{

}

void BT_MiddleNode::InitNode()
{
	BT_Node::InitNode();
	for (auto& iter : m_ChildNodes)
	{
		iter->InitNode();
	}

}

void BT_MiddleNode::AddChildNode(BT_Node* node)
{
	if (node == nullptr)
	{
		return;
	}
	BT_MiddleNode* PrevParentNode = node->GetParentNode();
	if (PrevParentNode)
	{
		PrevParentNode->DeleteChildNode(node);
	}
	m_ChildNodes.push_back(node);
	node->ThisIsMyParent(this);
}
void BT_MiddleNode::DeleteChildNode(BT_Node* node)
{
	if (m_ChildNodes.empty())
	{
		return;
	}
	m_ChildNodes.remove(node);
}

void BT_MiddleNode::DeleteChildNode(const wstring& name)
{
	if (m_ChildNodes.empty())
	{
		return;
	}
	m_ChildNodes.remove_if([name](BT_Node* node)
	{
		if (node->GetNodeName() == name)
		{
			return true;
		}
		return false;
	});
}

void BT_MiddleNode::SetBoard(BT_InformationBoard* board)
{
	BT_Node::SetBoard(board);
	for (auto& iter : m_ChildNodes)
	{
		iter->SetBoard(board);
	}
}

BT_Node* BT_MiddleNode::GetChildNode(const wstring& name)
{
	auto childNode = find_if(m_ChildNodes.begin(), m_ChildNodes.end(), [name](BT_Node* node)
	{
		if (node->GetNodeName() == name)
		{
			return true;
		}
		return false;
	});
	if (childNode == m_ChildNodes.end())
	{
		return nullptr;
	}
	return *childNode;
}
const std::list<BT_Node*>& BT_MiddleNode::GetChildNodeList_c()
{
	return m_ChildNodes;
}

std::list<BT_Node*>* BT_MiddleNode::GetChildNodeList_p()
{
	return &m_ChildNodes;
}

const size_t BT_MiddleNode::GetListSize()
{
	return m_ChildNodes.size();
}
