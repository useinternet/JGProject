#pragma once
#include"../../EngineStatics/Engine.h"

class JGDeviceD;
class JGHLSLShaderDevice;
class JGBufferManager;
class JGRenderSuperClass;
class JGViewportD;
/*
Class : JGComponentMessage */
class ENGINE_EXPORT JGComponentMessage
{
public:
	static const char* ComponentID;
	UINT Flag;
};
class ENGINE_EXPORT ComponentBase
{
private:
	static const char* ComponentID;
	JGRenderSuperClass* m_RenderSuperClass;
public:
	ComponentBase();
	virtual ~ComponentBase();
	virtual void Receive(const JGComponentMessage& Message) = 0;
	virtual void BeginComponent() = 0;
	virtual void Tick(const float DeltaTime) = 0;
	virtual void Render() = 0;
	virtual void DestoryComponent() = 0;
	void InitComponent(JGRenderSuperClass* SuperClass);
protected:
	/*
	Exp : 컴포넌트 아이디를 등록합니다.
	@param : const type_info& ID : typeid(ComponentType)으로 파라미터를 전달한다. */
	static void RegisterComponentID(const type_info& ID);
	/*
	Exp : 컴포넌트아이디를 가져옵니다. */
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
};