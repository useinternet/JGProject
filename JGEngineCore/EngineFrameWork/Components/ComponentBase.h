#pragma once
#include"../../EngineStatics/Engine.h"

class JGDevice;
class JGHLSLShaderDevice;

/*
Class : JGComponentMessage */
class JGComponentMessage
{
public:
	static const char* ComponentID;
	UINT Flag;
};
class ComponentBase
{
private:
	static const char* ComponentID;
public:
	ComponentBase();
	virtual ~ComponentBase();
	virtual void Receive(const JGComponentMessage& Message) = 0;
	virtual void BeginComponent() = 0;
	virtual void Tick(const float DeltaTime) = 0;
	virtual void Render() = 0;
	virtual void DestoryComponent() = 0;

protected:
	/*
	Exp : 컴포넌트 아이디를 등록합니다.
	@param : const type_info& ID : typeid(ComponentType)으로 파라미터를 전달한다. */
	static void RegisterComponentID(const type_info& ID);
	/*
	Exp : 컴포넌트아이디를 가져옵니다. */
	static const char* GetID();
};