#pragma once
#include"Component.h"


class ENGINE_EXPORT BT_Node : public Component
{
private:
	std::list<std::shared_ptr<BT_Node>> m_ChildNodes;
protected:
	enum class EBTNodeType
	{
		None,
		Task,
		Root,
		Sequence,
		Selector
	};
public:
	BT_Node();
	virtual ~BT_Node();

protected:
	virtual EBTNodeType GetNodeType();
};
