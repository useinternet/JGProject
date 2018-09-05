#include"ComponentBase.h"
#include"../../RenderSystem/JGDeviceD.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGBufferManager.h"
#include"../../RenderSystem/JGViewportD.h"
#include"../../EngineStatics/JGSuperClass.h"
JGSuperClass* ComponentBase::m_SuperClass;
ComponentBase::ComponentBase()
{
}

ComponentBase::~ComponentBase()
{
}

void ComponentBase::InitComponent(JGSuperClass* SuperClass)
{
	m_SuperClass = SuperClass;
}
void ComponentBase::RegisterComponentID(const type_info & ID)
{
	ComponentID = ID.name();
}

const char * ComponentBase::GetID()
{
	return ComponentID;
}


JGDeviceD* ComponentBase::GetDevice()
{
	return m_SuperClass->GetDevice();
}
JGHLSLShaderDevice* ComponentBase::GetHLSLDevice()
{
	return m_SuperClass->GetHLSLDevice();
}
JGBufferManager* ComponentBase::GetBufferManager()
{
	return m_SuperClass->GetBufferManager();
}

JGViewportD* ComponentBase::GetViewport()
{
	return m_SuperClass->GetViewport();
}

JGCommandManager* ComponentBase::GetCommandManager()
{
	return m_SuperClass->GetCommandManager();
}

