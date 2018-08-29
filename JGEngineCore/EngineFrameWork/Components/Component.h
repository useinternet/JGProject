#pragma once
#include"ComponentBase.h"
#include"../../EngineStatics/JMath/JGVector3D.h"
#include"../../EngineStatics/JMath/JGVector2D.h"
#include"../../EngineStatics/JMath/JGScale3D.h"
#include"../../EngineStatics/JMath/JGScale2D.h"
#include"../../EngineStatics/JMath/JGRotator.h"
#include"../../EngineStatics/JMath/JGMatrix.h"
#include"../../EngineStatics/JMath/JGMath.h"
#include"../../EngineStatics/JMath/JGAngle2D.h"

class JGDeviceD;
class JGHLSLShaderDevice;

class ENGINE_EXPORT Component : public ComponentBase
{
	std::vector<Component*> m_vChildComponent;
	Component* m_ParentComponent = nullptr;
	std::wstring m_ComponentName = TT("None");

public:
	Component();
	virtual ~Component();


	/*
	Exp : 메세지를 받는다. ( 부모 클래스의 Receive도 호출해야한다. ) */
	virtual void Receive(const JGComponentMessage& Message) override;
	/*
	Exp : 컴포넌트가 생성되고 최초 실행되는 함수이다. ( 부모 클래스의 BeginComponent도 호출해야한다. ) */
	virtual void BeginComponent() override;
	/*
	Exp : 매 프레임 한번 실행되는 함수이다. ( 부모 클래스의 Tick도 호출해야한다. ) */
	virtual void Tick(const float DeltaTime) override;
	/*
	Exp : 렌더링 한다. ( 상속받지 마셈. ) */
	virtual void Render() override;
	/*
	Exp :  컴포넌트가 파괴되기전 한번 실행되는 함수이다. ( 부모 클래스의 DestoryComponent도 호출해야한다. ) */
	virtual void DestoryComponent() override;


public:
	/*
	Exp : 컴포넌트 이름을 등록한다. (컴포넌트를 탐색할때 쓰인다.)
	@param : const wstring& Name : 등록할 컴포넌트 이름 */
	void RegisterName(const std::wstring& Name);
	/*
	Exp : 컴포넌트 이름을 가져온다. */
	const std::wstring& GetComponentName();

	/*
	Exp : 컴포넌트의 자식을 추가한다. ( 만약 추가하는 자식 컴포넌트의 부모가 존재한다면
	원래 부모 컴포넌트에 있던 자식컴포넌트는 제거되고 현재 추가할려는 부모 컴포넌트에 추가된다.
	또한 자식컴포넌트의 부모는 지금 추가할려는 부모 컴포넌트가된다.)
	@param : Component* Child : 자식이 될 컴포넌트 */
	void AddChild(Component* Child);
	/*
	Exp : 현재 컴포넌트의 부모 컴포넌트를 가져온다. */
	Component* GetParent();
	/*
	Exp : 현재 컴포넌트가 가지고있는 자식 컴포넌트들의 배열(vector형식)을 가져온다. */
	std::vector<Component*>& GetChildArray();

	/*
	Exp : 해당 컴포넌트 ID에 해당하는 자식컴포넌트들을 배열(vector)형식으로 가져온다. */
	std::vector<Component*> GetChildArray(const char* ComponentID);
	/*
	Exp : 해당 컴포넌트 ID에 해당하는 자식 컴포넌트를 가져온다. (여러개가 있다면 맨 처음 해당하는 컴포넌트를 가져온다. )
	@param : const char* ComponentID : 컴포넌트 아이디 */
	Component* GetChild(const char* ComponentID);
	/*
	Exp : 해당 컴포넌트 이름에 해당하는 자식 컴포넌트를 가져온다.( 같은 이름이 없다는 전제하에 가져온다.) 
	@param : const wstring& ComponentName : 컴포넌트 이름 */
	Component* GetChild(const std::wstring& ComponentName);
	/*
	Exp : index위치에 존재하는 자식 컴포넌트를 가져온다. 
	@param : const size_T index : 자식 컴포넌트 인덱스 */
	Component* GetChild(const size_t index);
private:
	void SetParent(Component* Parent);
};