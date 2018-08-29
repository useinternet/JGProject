#pragma once
#include"../../EngineStatics/Engine.h"

class JGDeviceD;
class JGHLSLShaderDevice;
class JGBufferManager;
class JGRenderSuperClass;
class JGComponentMessage;
class JGViewportD;
/*
Class : ObjectBase */
class ENGINE_EXPORT ObjectBase
{
private:
	static const char* ObjectID;
	JGRenderSuperClass* m_RenderSuperClass;
public:
	ObjectBase();
	virtual ~ObjectBase();
	virtual void BeginObject() = 0;
	virtual void Send(JGComponentMessage& Message) = 0;
	virtual void Tick(const float DeltaTime) = 0;
	virtual void Render() = 0;
	virtual void DestroyObject() = 0;
	void InitObejct(JGRenderSuperClass* SuperClass);
protected:
	/*
	Exp : 오브젝트 타입 고유 아이디를 등록한다. 
	@param : const type_info& ID : typeid(ObjecType) 형식으로 파라미터를 전달. */
	static void RegisterObjectID(const type_info& ID);
	/*
	Exp : 아이디를 얻어온다. */
	static const char* GetID();

	/*
	Exp : JGDevice 클래스를 얻어온다. */
	JGDeviceD* GetDevice();
	/*
	Exp : JGHLSLShaderDevice 클래스를 얻어온다. */
	JGHLSLShaderDevice* GetHLSLDevice();
	/*
	Exp : JGBufferManager 클래스를 얻어온다. */
	JGBufferManager* GetBufferManager();
	/*
	Exp : JGViewportD 클래스를 얻어온다. */
	JGViewportD* GetViewport();
	/*
	Exp : JGViewportD 클래스를 얻어온다. */
	JGRenderSuperClass* GetRenderSuperClass();

};