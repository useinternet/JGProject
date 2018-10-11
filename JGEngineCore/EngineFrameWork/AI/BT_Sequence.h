#pragma once
#include"BT_MiddleNode.h"


// Task : 말단 노드( 이벤트 발생 )
// Selector : 차례대로 순차 탐색중 false라면 게속 오른쪽 탐색.. 하다가
// true를 반환하면 루프에서 빠져나온다.
// Sequence : 자식노드중 모두 true여야만 루프에서 빠져나온다.

class ENGINE_EXPORT BT_Sequence : public BT_MiddleNode
{
private:
	EBTNodeType m_NodeType;
	size_t m_CurrentCount = 0;
	bool   m_bChildNodeAllPass = true;
public:
	BT_Sequence();
	virtual ~BT_Sequence();
	virtual void InitNode() override;
	virtual void Behavior(const float DeltaTime) override;
	virtual EBTNodeType GetNodeType() override;
};