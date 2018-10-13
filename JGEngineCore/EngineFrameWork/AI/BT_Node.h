#pragma once
#include"../../EngineStatics/Engine.h"

enum class ENGINE_EXPORT EBTNodeType
{
	None,
	Task,
	Root,
	Sequence,
	Selector
};

// Task 노드한테 필요한거      이름, 타입, 인포메이션 보드, 부모 노드
class ENGINE_EXPORT BT_Node
{
private:
	std::wstring        m_NodeName = TT("None");

	class BT_MiddleNode* m_ParentNode = nullptr;
protected:

	bool m_IsPass = false;
public:
	BT_Node();
	virtual ~BT_Node();
	virtual void InitNode();
	virtual void Behavior(const float DeltaTime);
	void RegisterName(const std::wstring& name);           // 이름 등록


	bool IsPass();
	void Pass();
	void NoPass();

	BT_MiddleNode* GetParentNode();                              // 부모노드 가져오기
	virtual EBTNodeType GetNodeType();
	const std::wstring& GetNodeName();      
	void ThisIsMyParent(BT_MiddleNode* node);                    // 부모 노드 설정
};
