#include"JGRenderSuperClass.h"
#include"JGDeviceD.h"
#include"JGHLSLShaderDevice/JGBufferManager.h"
#include"JGViewportD.h"
#include"JGHLSLShaderDevice/JGHLSLShaderDevice.h"


void JGRenderSuperClass::LinkPointer(JGDeviceD* Device, JGViewportD* Viewport, JGHLSLShaderDevice* HLSLDevice, JGBufferManager* BufferManager)
{
	m_Device = Device;
	m_Viewport = Viewport;
	m_HLSLDevice = HLSLDevice;
	m_BufferManager = BufferManager;
}

JGDeviceD* JGRenderSuperClass::GetDevice()
{
	return m_Device;
}
JGViewportD* JGRenderSuperClass::GetViewport()
{
	return m_Viewport;
}
JGHLSLShaderDevice* JGRenderSuperClass::GetHLSLDevice()
{
	return m_HLSLDevice;
}
JGBufferManager* JGRenderSuperClass::GetBufferManager()
{
	return m_BufferManager;
}