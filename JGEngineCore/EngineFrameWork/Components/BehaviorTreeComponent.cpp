#include"BehaviorTreeComponent.h"
#include"BT_Root.h"
using namespace std;
BehaviorTreeComponent::BehaviorTreeComponent()
{
	RegisterComponentID(typeid(this));
	m_TreeRoot = make_unique<BT_Root>();
}

BehaviorTreeComponent::~BehaviorTreeComponent()
{

}

void BehaviorTreeComponent::Tick(const float DeltaTime)
{

}
