#pragma once
#include"../../EngineStatics/Engine.h"

class JGDeviceD;
class JGHLSLShaderDevice;
class JGBufferManager;
class JGSuperClass;
class JGViewportD;
class JGCommandManager;
class SoundSystem;
/*
Class : JGComponentMessage */
class ENGINE_EXPORT JGComponentMessage
{
public:
	const char* ComponentID;
	UINT Flag;
};
class ENGINE_EXPORT ComponentBase
{
private:
	const char* ComponentID;
	static JGSuperClass* m_SuperClass;
public:
	ComponentBase();
	virtual ~ComponentBase();
	virtual void Receive(const JGComponentMessage& Message) = 0;
	virtual void BeginComponent() = 0;
	virtual void Tick(const float DeltaTime) = 0;
	virtual void Render() = 0;
	virtual void DestoryComponent() = 0;
	void InitComponent(JGSuperClass* SuperClass);
protected:
	/*
	Exp : 컴포넌트 아이디를 등록합니다.
	@param : const type_info& ID : typeid(ComponentType)으로 파라미터를 전달한다. */
	void RegisterComponentID(const type_info& ID);
	/*
	Exp : 컴포넌트아이디를 가져옵니다. */
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
	Exp : CommandManager 클래스를 얻어온다. */
	JGCommandManager* GetCommandManager();
	/*
	Exp : SoundSystem 을 불러온다. */
	SoundSystem* GetSoundSystem();
};