#pragma once
#include"Component.h"
class BT_Root;
// Task : 말단 노드( 이벤트 발생 )
// Selector : 차례대로 순차 탐색중 false라면 게속 오른쪽 탐색.. 하다가
// true를 반환하면 루프에서 빠져나온다.
// Sequence : 자식노드중 모두 true여야만 루프에서 빠져나온다.
class ENGINE_EXPORT BehaviorTreeComponent : public Component
{
private:
	std::unique_ptr<BT_Root> m_TreeRoot;

public:
	BehaviorTreeComponent();
	virtual ~BehaviorTreeComponent();

	/*
	Exp : 비헤이비어 트리는 전의 컴포넌트 Tick을 쓰지 않고 새롭게 쓴다. */
	virtual void Tick(const float DeltaTime) override;

};