#include"JGSuperClass.h"
#include"../RenderSystem/JGDeviceD.h"
#include"../RenderSystem/JGHLSLShaderDevice/JGBufferManager.h"
#include"../RenderSystem/JGViewportD.h"
#include"../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../InputSystem/JGCommandManager.h"


void JGSuperClass::LinkPointer(JGDeviceD* Device, JGViewportD* Viewport, JGHLSLShaderDevice* HLSLDevice, JGBufferManager* BufferManager,
	JGCommandManager* CommandManager)
{
	m_Device = Device;
	m_Viewport = Viewport;
	m_HLSLDevice = HLSLDevice;
	m_BufferManager = BufferManager;
	m_CommandManager = CommandManager;
}

JGDeviceD* JGSuperClass::GetDevice()
{
	return m_Device;
}
JGViewportD* JGSuperClass::GetViewport()
{
	return m_Viewport;
}
JGHLSLShaderDevice* JGSuperClass::GetHLSLDevice()
{
	return m_HLSLDevice;
}
JGBufferManager* JGSuperClass::GetBufferManager()
{
	return m_BufferManager;
}
JGCommandManager* JGSuperClass::GetCommandManager()
{
	return m_CommandManager;
}
