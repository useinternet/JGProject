#pragma once
#include"Component.h"

#include"../AI/BT_Selector.h"
#include"../AI/BT_Sequence.h"
#include"../AI/BT_Task.h"
// Task : 말단 노드( 이벤트 발생 )
// Selector : 차례대로 순차 탐색중 false라면 게속 오른쪽 탐색.. 하다가
// true를 반환하면 루프에서 빠져나온다.
// Sequence : 자식노드중 모두 true여야만 루프에서 빠져나온다.
class ENGINE_EXPORT BehaviorTreeComponent : public Component
{
private:
	std::unique_ptr<class BT_Root> m_Root;
	std::vector<std::unique_ptr<BT_Node>> m_vBTNodes;
public:
	BehaviorTreeComponent();
	virtual ~BehaviorTreeComponent();
	virtual void BeginComponent(World* world) override;
	virtual void MakeAITreeSpace();

	template<typename UserTask>
	UserTask* CreateTask(const std::wstring& nodeName);
	BT_Selector* CreateSelector(const std::wstring& nodeName);
	BT_Sequence* CreateSequence(const std::wstring& nodeName);
	BT_MiddleNode* GetRoot();
	/*
	Exp : 비헤이비어 트리는 전의 컴포넌트 Tick을 쓰지 않고 새롭게 쓴다. */
	virtual void Tick(const float DeltaTime) override;

};
template<typename UserTask>
inline UserTask* BehaviorTreeComponent::CreateTask(const std::wstring& nodeName)
{
	std::unique_ptr<UserTask> task = make_unique<UserTask>();
	task->InitNode();
	task->RegisterName(nodeName);

	UserTask* result = task.get();

	m_vBTNodes.push_back(move(task));

	return result;
}
