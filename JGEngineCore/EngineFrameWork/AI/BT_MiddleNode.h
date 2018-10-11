#pragma once
#include"BT_Node.h"

class ENGINE_EXPORT BT_MiddleNode : public BT_Node
{
private:
	std::list<BT_Node*> m_ChildNodes;

public:
	BT_MiddleNode();
	virtual ~BT_MiddleNode();
	virtual void InitNode() override;
	void AddChildNode(BT_Node* node);                      // 자식 추가
	void DeleteChildNode(BT_Node* node);
	void DeleteChildNode(const std::wstring& name);
	virtual void SetBoard(BT_InformationBoard* board) override;
	BT_Node* GetChildNode(const std::wstring& name);       // 자식 가져오기(이름)     
	const std::list<BT_Node*>& GetChildNodeList_c();       // 자식 배열 가져오기(const)
	std::list<BT_Node*>* GetChildNodeList_p();             // 자식 배열 가져오기(pointer)
	const size_t GetListSize();
};