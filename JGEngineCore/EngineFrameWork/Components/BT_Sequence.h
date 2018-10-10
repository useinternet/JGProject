#pragma once
#include"BT_Node.h"

class ENGINE_EXPORT BT_Sequence : public BT_Node
{
private:
	EBTNodeType m_NodeType;

public:
	BT_Sequence();
	virtual ~BT_Sequence();

	virtual void Tick(const float DeltaTime) override;

protected:
	virtual EBTNodeType GetNodeType() override;
};