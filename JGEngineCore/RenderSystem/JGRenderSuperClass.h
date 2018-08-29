#pragma once
#include"../EngineStatics/Engine.h"

class JGDeviceD;
class JGViewportD;
class JGHLSLShaderDevice;
class JGBufferManager;


class ENGINE_EXPORT JGRenderSuperClass
{
private:
	JGDeviceD* m_Device;
	JGViewportD* m_Viewport;
	JGHLSLShaderDevice* m_HLSLDevice;
	JGBufferManager* m_BufferManager;
public:
	void LinkPointer(JGDeviceD* Device, JGViewportD* Viewport, JGHLSLShaderDevice* HLSLDevice, JGBufferManager* BufferManager);

	JGDeviceD* GetDevice();
	JGViewportD* GetViewport();
	JGHLSLShaderDevice* GetHLSLDevice();
	JGBufferManager* GetBufferManager();
};