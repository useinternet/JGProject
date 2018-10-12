#include"BehaviorTreeComponent.h"
#include"../AI/BT_Root.h"
using namespace std;
BehaviorTreeComponent::BehaviorTreeComponent()
{
	RegisterComponentID(typeid(this));
	m_Root = make_unique<BT_Root>();
}

BehaviorTreeComponent::~BehaviorTreeComponent()
{

}

void BehaviorTreeComponent::BeginComponent(World* world)
{
	m_Root->InitNode();
	m_Root->RegisterName(GetComponentName()+TT("_BT_Root"));
	MakeAITreeSpace();
}
void BehaviorTreeComponent::MakeAITreeSpace()
{

}
BT_Selector* BehaviorTreeComponent::CreateSelector(const wstring& nodeName)
{
	unique_ptr<BT_Selector> selector = make_unique<BT_Selector>();
	selector->InitNode();
	selector->RegisterName(nodeName);
	BT_Selector* result = selector.get();


	m_mBTNodes.insert(
		pair<const std::wstring, std::unique_ptr<BT_Node>>(nodeName, move(selector)));

	return result;
}

BT_Sequence* BehaviorTreeComponent::CreateSequence(const wstring& nodeName)
{
	unique_ptr<BT_Sequence> sequence = make_unique<BT_Sequence>();
	sequence->InitNode();
	sequence->RegisterName(nodeName);
	BT_Sequence* result = sequence.get();

	m_mBTNodes.insert(
		pair<const std::wstring, std::unique_ptr<BT_Node>>(nodeName, move(sequence)));

	return result;
}

BT_MiddleNode* BehaviorTreeComponent::GetRoot()
{
	return m_Root.get();
}

void BehaviorTreeComponent::Tick(const float DeltaTime)
{
	m_Root->Behavior(DeltaTime);
}
