#pragma once
#include"../EngineStatics/Engine.h"

class JGDeviceD;
class JGViewportD;
class JGHLSLShaderDevice;
class JGBufferManager;
class JGCommandManager;
/*
Class : JGSuperClass
Exp : 렌더링 시스템에서 초기화한 장치들의 포인터 모음집!! */
class ENGINE_EXPORT JGSuperClass
{
private:
	JGDeviceD* m_Device;
	JGViewportD* m_Viewport;
	JGHLSLShaderDevice* m_HLSLDevice;
	JGBufferManager* m_BufferManager;
	JGCommandManager* m_CommandManager;
public:
	void LinkPointer(JGDeviceD* Device, JGViewportD* Viewport, JGHLSLShaderDevice* HLSLDevice, JGBufferManager* BufferManager,
		JGCommandManager* CommandManager);

	JGDeviceD*          GetDevice();
	JGViewportD*        GetViewport();
	JGHLSLShaderDevice* GetHLSLDevice();
	JGBufferManager*    GetBufferManager();
	JGCommandManager*   GetCommandManager();
};