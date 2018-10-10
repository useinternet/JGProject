#pragma once
#include"BT_Node.h"


class ENGINE_EXPORT BT_Root : public BT_Node
{
private:
	EBTNodeType m_NodeType;

public:
	BT_Root();
	virtual ~BT_Root();

	virtual void Tick(const float DeltaTime) override;

protected:
	virtual EBTNodeType GetNodeType() override;
};