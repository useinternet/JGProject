#pragma once
#include"BT_Node.h"

class ENGINE_EXPORT BT_Selector : public BT_Node
{
private:
	EBTNodeType m_NodeType;
public:
	BT_Selector();
	virtual ~BT_Selector();


	virtual void Tick(const float DeltaTime) override;


protected:
	virtual EBTNodeType GetNodeType() override;
};