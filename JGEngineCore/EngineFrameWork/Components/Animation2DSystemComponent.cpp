#include"Animation2DSystemComponent.h"
#include"../Object/Object.h"
using namespace std;
Animation2DSystemComponent::Animation2DSystemComponent()
{
	RegisterComponentID(typeid(this));
}

Animation2DSystemComponent::~Animation2DSystemComponent()
{
}

void Animation2DSystemComponent::Tick(const float DeltaTime)
{
	Motivated2DComponent::Tick(DeltaTime);
	if (m_CurrentState == 2)
	{
		int num = 0;
	}
	m_mAnimation[m_CurrentState]->Tick(DeltaTime);
}

void Animation2DSystemComponent::Render()
{
	if (m_CurrentState == 2)
	{
		int num = 0;
	}
	m_mAnimation[m_CurrentState]->Render();
}
void Animation2DSystemComponent::LinkObject(Object* object)
{
	m_pLinkObject = object;
}

Object* Animation2DSystemComponent::GetLinkObject()
{
	
	return m_pLinkObject;
}

void Animation2DSystemComponent::SetCurrentState(const EnumState State)
{
	m_CurrentState = State;
}

EnumState Animation2DSystemComponent::GetCurrentState()
{
	return m_CurrentState;
}

AnimationMesh2DComponent* Animation2DSystemComponent::AddAnimation(EnumState State, const std::wstring& AnimationName)
{
	unique_ptr<AnimationMesh2DComponent> AnimationComponent = make_unique<AnimationMesh2DComponent>();
	AnimationComponent->RegisterName(AnimationName);
	AddChild(AnimationComponent.get());
	m_mAnimation.insert(pair<EnumState, shared_ptr<AnimationMesh2DComponent>>(State,move(AnimationComponent)));
	return m_mAnimation[State].get();
}

void Animation2DSystemComponent::AddNotifyEvent(const wstring & NotifyName, EnumState State, const size_t Frame, const std::function<void()>& Event)
{
	SAnimNotify Notify;
		Notify.Frame = Frame;
		Notify.State = State;
		Notify.Event = Event;
		m_mNotifys.insert(std::pair<const wstring, SAnimNotify>(NotifyName, Notify));
}

AnimationMesh2DComponent* Animation2DSystemComponent::GetAnimation(EnumState State)
{
	return m_mAnimation[State].get();
}
