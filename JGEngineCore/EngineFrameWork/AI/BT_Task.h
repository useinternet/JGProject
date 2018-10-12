#pragma once
#include"BT_Node.h"

class ENGINE_EXPORT BT_Task : public BT_Node
{
private:
	EBTNodeType m_NodeType;
	class BT_InformationBoard* m_Board = nullptr;
public:
	BT_Task();
	virtual ~BT_Task();
	
	virtual void Behavior(const float DeltaTime) override;
	bool ExcuteTask(const float DeltaTime);
	virtual EBTNodeType GetNodeType() override;
	void SetBoard(BT_InformationBoard* board);
	BT_InformationBoard* GetBoard();

};