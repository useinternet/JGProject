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

void Animation2DSystemComponent::BeginComponent(World* world)
{
	Motivated2DComponent::BeginComponent(world);
	for (auto& iter : m_mAnimation)
	{
		iter.second->SetOwnerObject(GetOwnerObject());
	}
}

void Animation2DSystemComponent::Tick(const float DeltaTime)
{
	Motivated2DComponent::Tick(DeltaTime);

	for (auto& notify : m_mNotifys)
	{
		SAnimNotify ntfy = notify.second;
		if (ntfy.bEventPlay && !m_mAnimation[ntfy.State]->AnimationIsFrame(ntfy.Frame))
		{
			notify.second.bEventPlay = false;
		}
		if (m_mAnimation[ntfy.State]->IsPlaying() && m_mAnimation[ntfy.State]->AnimationIsFrame(ntfy.Frame))
		{
			if (!ntfy.bEventPlay)
			{
				ntfy.Event();
				notify.second.bEventPlay = true;
			}
		}
	
	}
}

void Animation2DSystemComponent::Render()
{
	m_mAnimation[m_CurrentState]->Render();
}
void Animation2DSystemComponent::SetCurrentState(const EnumState State)
{

	for (auto& iter : m_mAnimation)
	{
		if (iter.first != State)
		{
			iter.second->Stop();
			iter.second->InitAnimationSetting();
		}
	}
	m_CurrentState = State;
	m_mAnimation[m_CurrentState]->Play();
}
EnumState Animation2DSystemComponent::GetCurrentState()
{
	return m_CurrentState;
}
AnimationMesh2DComponent* Animation2DSystemComponent::AddAnimation(const EnumState State, const std::wstring& AnimationName)
{
	unique_ptr<AnimationMesh2DComponent> AnimationComponent = make_unique<AnimationMesh2DComponent>();
	AnimationComponent->SetOwnerObject(GetOwnerObject());
	AnimationComponent->RegisterName(AnimationName);
	AddChild(AnimationComponent.get());

	m_mAnimation.insert(pair<EnumState, shared_ptr<AnimationMesh2DComponent>>(State,move(AnimationComponent)));


	return m_mAnimation[State].get();
}

void Animation2DSystemComponent::AddNotifyEvent(const wstring & NotifyName, const EnumState State, const size_t Frame, const std::function<void()>& Event)
{
	SAnimNotify Notify;
	Notify.Frame = Frame;
	Notify.State = State;
	Notify.Event = Event;
	m_mNotifys.insert(std::pair<const wstring, SAnimNotify>(NotifyName, Notify));
}

AnimationMesh2DComponent* Animation2DSystemComponent::GetAnimation(const EnumState State)
{
	return m_mAnimation[State].get();
}
