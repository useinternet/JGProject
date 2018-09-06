#pragma once
#include"AnimationMesh2DComponent.h"
class Object;
typedef unsigned int EnumState;
/*
Exp : 오브젝트 상태에따라 애니메이션 만들고 싶을때.. 쓰는 컴포넌트 */
class ENGINE_EXPORT Animation2DSystemComponent : public Motivated2DComponent
{
private:
	struct SAnimNotify
	{
		EnumState State;
		size_t Frame;
		std::function<void()> Event;
	};

private:
	std::map<EnumState,std::shared_ptr<AnimationMesh2DComponent>> m_mAnimation;
	std::map<const std::wstring, SAnimNotify> m_mNotifys;
	Object* m_pLinkObject = nullptr;
	EnumState m_CurrentState;
public:
	Animation2DSystemComponent(const Animation2DSystemComponent& copy) = delete;
	Animation2DSystemComponent();
	virtual ~Animation2DSystemComponent();
	virtual void Tick(const float DeltaTime) override;
	virtual void Render() override;

	/*
	Exp : 이 애니메이션의 정보를 해당 오브젝트와 연결한다. */
	void LinkObject(Object* object);
	/*
	Exp : 이 애니메이션에 연결된 오브젝트를 가져온다. */
	Object* GetLinkObject();


	/*
	애니메이션 현재 상태를 설정한다. */
	void SetCurrentState(const EnumState State);

	/*
	애니메이션 현재 상태를 가져온다. */
	EnumState GetCurrentState();
	/*
	Exp : 해당 상태일때 애니메이션 추가..( 여기서 이 컴포넌트에 자식으로 추가..그래야 위치값이 연동되니까) */
	AnimationMesh2DComponent* AddAnimation(EnumState State,const std::wstring& AnimationName);
	/*
	Exp : 알림 이벤트 추가하기 */
	void AddNotifyEvent(const std::wstring& NotifyName, EnumState State, const size_t Frame, const std::function<void()>& Event);




	AnimationMesh2DComponent* GetAnimation(EnumState State);
};
