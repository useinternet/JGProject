#pragma once
#include"BT_Node.h"

class ENGINE_EXPORT BT_Task : public BT_Node
{
private:
	EBTNodeType m_NodeType;
public:
	BT_Task();
	virtual ~BT_Task();
	
	virtual void Behavior(const float DeltaTime) override;
	bool ExcuteTask(const float DeltaTime);
	virtual EBTNodeType GetNodeType() override;
};