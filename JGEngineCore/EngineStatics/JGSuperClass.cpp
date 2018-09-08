#include"JGSuperClass.h"
#include"../RenderSystem/JGDeviceD.h"
#include"../RenderSystem/JGHLSLShaderDevice/JGBufferManager.h"
#include"../RenderSystem/JGViewportD.h"
#include"../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../InputSystem/JGCommandManager.h"
#include"../SoundSystem/SoundSystem.h"

void JGSuperClass::LinkPointer(JGDeviceD* Device, JGViewportD* Viewport, JGHLSLShaderDevice* HLSLDevice, JGBufferManager* BufferManager,
	JGCommandManager* CommandManager, SoundSystem* soundSystem)
{
	m_Device = Device;
	m_Viewport = Viewport;
	m_HLSLDevice = HLSLDevice;
	m_BufferManager = BufferManager;
	m_CommandManager = CommandManager;
	m_SoundSystem = soundSystem;
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

SoundSystem* JGSuperClass::GetSoundSystem()
{
	return m_SoundSystem;
}
