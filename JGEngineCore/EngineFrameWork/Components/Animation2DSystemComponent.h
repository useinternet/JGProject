#pragma once
#include"AnimationMesh2DComponent.h"
class Object;
typedef unsigned int EnumState;


// 나중에 추가할 점
// 애니메이션 변동시.. 이전 애니메이션을 값들을 초기화 한다.



/*
Class : Animation2DSystemComponent
Exp : 애니메이션 시스템 컴포넌트( 상속 해서 애니메이션을 직접 구현해서쓴다.) 
@m map<EnumState,shared_ptr<AnimationMesh2DComponent>> m_mAnimation : 애니메이션 스프라이트 배열
@m map<const wstring, SAnimNotify> m_mNotifys : 애니메이션 알림 이벤트
@m Object* m_pLinkObject    : 연결된 오브젝트( 정보 공유 용 )
@m EnumState m_CurrentState : 현재 애니메이션 상태(사용자 정의)    */
class ENGINE_EXPORT Animation2DSystemComponent : public Motivated2DComponent
{
private:
	/*
	Struct : SAnimNotify
	@s EnumState State : 애니메이션 상태(사용자 정의)
	@s size_t Frame    : 알림 프레임 위치
	@s function<void()> Event : 노티파이(알림) 이벤트 함수*/
	struct SAnimNotify
	{
		EnumState State;
		size_t    Frame;
		std::function<void()> Event;
	};
private:
	std::map<EnumState,std::shared_ptr<AnimationMesh2DComponent>> m_mAnimation;
	std::map<const std::wstring, SAnimNotify> m_mNotifys;
	EnumState m_CurrentState;
public:
	Animation2DSystemComponent(const Animation2DSystemComponent& copy) = delete;
	Animation2DSystemComponent();
	virtual ~Animation2DSystemComponent();
	/// Component 에 상속 받은 가상 함수들..(자세한건 Component 클래스 참조)
	virtual void BeginComponent(World* world) override;
	virtual void Tick(const float DeltaTime) override;
	virtual void Render() override;
	//
	/*
	Exp : 이 애니메이션의 정보를 해당 오브젝트와 연결한다. */
	void LinkObject(Object* object);
	/*
	Exp : 이 애니메이션에 연결된 오브젝트를 가져온다. */
	Object* GetLinkObject();
	/*
	Exp : 애니메이션 현재 상태를 설정한다. */
	void SetCurrentState(const EnumState State);
	/*
	Exp : 애니메이션 현재 상태를 가져온다. */
	EnumState GetCurrentState();
	/*
	Exp : 해당 상태일때 애니메이션 추가..( 여기서 이 컴포넌트에 자식으로 추가..그래야 위치값이 연동되니까) 
	@param const EnumState State : 애니메이션 상태(사용자 정의) 
	@param const wstring& AnimationName : 애니메이션 이름. */
	AnimationMesh2DComponent* AddAnimation(const EnumState State,const std::wstring& AnimationName);
	/*
	Exp : 알림 이벤트 추가하기 
	@param const wstring&          NotifyName : 알림 이벤트 이름.
	@param const EnumState         State      : 알림을 추가할 애니메이션 상태
	@param const function<void()>& Event      : 알림 이벤트 함수*/
	void AddNotifyEvent(const std::wstring& NotifyName, const EnumState State, const size_t Frame, const std::function<void()>& Event);
	/*
	Exp : 해당 애니메이션 상태의 스프라이트를 가져온다. */
	AnimationMesh2DComponent* GetAnimation(const EnumState State);
};
