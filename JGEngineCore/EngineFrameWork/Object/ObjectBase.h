#pragma once
#include"../../EngineStatics/Engine.h"

class JGDeviceD;
class JGHLSLShaderDevice;
class JGBufferManager;
class JGSuperClass;
class JGComponentMessage;
class JGViewportD;
class World;
/*
Class : ObjectBase */
class ENGINE_EXPORT ObjectBase
{
private:
	const char* ObjectID;
	int ZOrder = 0;
	static JGSuperClass* m_RenderSuperClass;
public:
	ObjectBase();
	virtual ~ObjectBase();
	virtual void BeginObject(World* world) = 0;
	virtual void Send(JGComponentMessage& Message) = 0;
	virtual void Tick(const float DeltaTime) = 0;
	virtual void Render() = 0;
	virtual void EndObject() = 0;
	void InitObejct(JGSuperClass* SuperClass);


	/*
	Exp : ZOrder 값을 불러온다. */
	int GetZOrder();
	/*
	Exp : ZOrder 값을 설정한다. */
	void SetZOrder(int Num);
protected:
	/*
	Exp : 오브젝트 타입 고유 아이디를 등록한다. 
	@param : const type_info& ID : typeid(ObjecType) 형식으로 파라미터를 전달. */
	void RegisterObjectID(const type_info& ID);
	/*
	Exp : 아이디를 얻어온다. */
	const char* GetID();

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
	JGSuperClass* GetRenderSuperClass();


};