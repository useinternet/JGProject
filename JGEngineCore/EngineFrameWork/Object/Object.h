#pragma once
#include"ObjectBase.h"
#include"../Components/MotivatedComponent.h"

class JGDeviceD;
class JGHLSLShaderDevice;
class World;
/*
EnumClass : 오브젝트 상태 */
enum class EObjectState
{
	Active = 0,
	Behind    = 1,
	Destory   = 2
};
/*
Class : Object 
@m std::vector<std::unique_ptr<Component>>  m_vComponents : 컴포넌트 배열 ( 주소 유지용 )
@m Component* m_RootComponent : 계층 구조 컴포넌트 시작 지점 */
class ENGINE_EXPORT Object : public ObjectBase
{
private:
	std::vector<std::shared_ptr<Component>> m_vComponents;
	Component* m_RootComponent = nullptr;
	//
	bool m_bIsFirst = true;
	EObjectState m_ObjectState;
	World* m_pWorld = nullptr;
public:
	Object();
	virtual ~Object();

	/*
	Exp : 오브젝트가 생성되고 나서 최초 한번 실행되는 이벤트입니다. (무조건 부모 클래스의 BeginObject를 호출한다.)*/
	virtual void BeginObject(World* world) override;
	/*
	Exp : 컴포넌트 메세지를 보냅니다. 
	@param : JGComponentMessage& Message : 컴포넌트 메시지 클래스 (무조건 부모 클래스의 Send를 호출한다.)*/
	virtual void Send(JGComponentMessage& Message) override;
	/*
	Exp : 1프레임당 한번 호출 되는 이벤트 입니다. (무조건 부모 클래스의 Tick을 호출한다.)*/
	virtual void Tick(const float DeltaTime) override;
	/*
	Exp : 렌더링 합니다.(상속받지 말자) */
	virtual void Render() override;
	/*
	Exp : 오브젝트를 파괴하기전 최초 한번 실행되는 이벤트입니다.(무조건 부모 클래스의 DestroyObject를 호출한다.) */
	virtual void EndObject() override;
	/*
	Exp : 오브젝트가 속한 월드를 불러온다. */
	World* GetWorld();
	/*
	Exp : 오브젝트 상태를 가져온다.*/
	EObjectState GetObjectState();
	/*
	Exp : 오브젝트를 파괴한다. */
	void DestoryObject();
	/*
	Exp : 오브젝트를 숨긴다. */
	void BehindObject();
	/*
	Exp : 오브젝트를 활성화시킨다. */
	void ActiveObject();
protected:
	template<typename ComponentType>
	ComponentType* RegisterComponentInObject(const std::wstring& ComponentName);
};

template<typename ComponentType>
inline ComponentType* Object::RegisterComponentInObject(const std::wstring& ComponentName)
{
	// 만약 처음 등록하는 컴포넌트라면..
	if (m_bIsFirst)
	{
		m_bIsFirst = false;
		// 루트 컴포넌트를 만든다.
		std::unique_ptr<Component> RootComponent = std::make_unique<MotivatedComponent>();
		RootComponent->InitComponent(GetRenderSuperClass());
		m_RootComponent = RootComponent.get();
		m_vComponents.push_back(move(RootComponent));
	}
	// 컴포넌트 생성
	std::unique_ptr<Component> component = std::make_unique<ComponentType>();
	component->RegisterName(ComponentName);
	// 계층 구조 컴포넌트인지 체크한다.
	MotivatedComponent* Check = dynamic_cast<MotivatedComponent*>(component.get());
	// 계층 구조 컴포넌트라면.
	if (Check)
	{
		// 루트 컴포넌트 자식으로 컴포넌트를 추가한다.
		ComponentType* result = dynamic_cast<ComponentType*>(Check);
		m_RootComponent->AddChild(component.get());
		m_vComponents.push_back(move(component));
		return result;
	}
	else
	{
		// 아니라면 그냥 추가한다.
		ComponentType* result = dynamic_cast<ComponentType*>(component.get());
		m_vComponents.push_back(move(component));
		return result;
	}
}