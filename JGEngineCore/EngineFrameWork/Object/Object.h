#pragma once
#include"ObjectBase.h"

class Component;
class JGDeviceD;
class JGHLSLShaderDevice;
/*
Class : Object 
@m std::vector<std::unique_ptr<Component>>  m_vComponents : 컴포넌트 배열 ( 주소 유지용 )
@m Component* m_RootComponent : 계층 구조 컴포넌트 시작 지점 */
class Object : public ObjectBase
{
private:
	std::vector<std::unique_ptr<Component>>  m_vComponents;
	Component* m_RootComponent;
	//
	bool m_bIsFirst = true;

public:
	Object();
	virtual ~Object();

	/*
	Exp : 오브젝트가 생성되고 나서 최초 한번 실행되는 이벤트입니다. (무조건 부모 클래스의 BeginObject를 호출한다.)*/
	virtual void BeginObject() override;
	/*
	Exp : 컴포넌트 메세지를 보냅니다. 
	@param : JGComponentMessage& Message : 컴포넌트 메시지 클래스 (무조건 부모 클래스의 Send를 호출한다.)*/
	virtual void Send(JGComponentMessage& Message) override;
	/*
	Exp : 1프레임당 한번 호출 되는 이벤트 입니다. (무조건 부모 클래스의 Tick을 호출한다.)*/
	virtual void Tick(const float DeltaTime) override;
	/*
	Exp : 렌더링 합니다.(상속받지 말자) */
	virtual void Render(JGDevice* Device, JGHLSLShaderDevice* HlslDevice) override;
	/*
	Exp : 오브젝트를 파괴하기전 최초 한번 실행되는 이벤트입니다.(무조건 부모 클래스의 DestroyObject를 호출한다.) */
	virtual void DestroyObject() override;

protected:
	template<typename ComponentType>
	ComponentType* RegisterComponentInObject(std::wstring& ComponentName);
};

