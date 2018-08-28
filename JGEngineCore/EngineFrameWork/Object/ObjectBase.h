#pragma once
#include"../../EngineStatics/Engine.h"

class JGDevice;
class JGHLSLShaderDevice;
class JGComponentMessage;

/*
Class : ObjectBase */
class ObjectBase
{
private:
	static const char* ObjectID;
public:
	ObjectBase();
	virtual ~ObjectBase();

	virtual void BeginObject() = 0;
	virtual void Send(JGComponentMessage& Message) = 0;
	virtual void Tick(const float DeltaTime) = 0;
	virtual void Render(JGDevice* Device, JGHLSLShaderDevice* HlslDevice) = 0;
	virtual void DestroyObject() = 0;
protected:
	/*
	Exp : 오브젝트 타입 고유 아이디를 등록한다. 
	@param : const type_info& ID : typeid(ObjecType) 형식으로 파라미터를 전달. */
	static void RegisterObjectID(const type_info& ID);
	/*
	Exp : 아이디를 얻어온다. */
	static const char* GetID();
};