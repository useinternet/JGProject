#pragma once
#include"BT_MiddleNode.h"
// 정보 보드를 모든 자식노드에게 돌린다.

class ENGINE_EXPORT BT_Root : public BT_MiddleNode
{
private:
	EBTNodeType m_NodeType;
	size_t      m_CurrentCount = 0;
public:
	BT_Root();
	virtual ~BT_Root();
	virtual void Behavior(const float DeltaTime) override;
	virtual EBTNodeType GetNodeType() override;
};