#pragma once
#include"BT_MiddleNode.h"

// false 면 게속 다음 노드 탐색 true면 벗어 나온다.
class ENGINE_EXPORT BT_Selector : public BT_MiddleNode
{
private:
	EBTNodeType m_NodeType;
	size_t      m_CurrentCount = 0;
public:
	BT_Selector();
	virtual ~BT_Selector();
	virtual void InitNode() override;
	virtual void Behavior(const float DeltaTime) override;
	virtual EBTNodeType GetNodeType() override;
};