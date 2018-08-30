#include"ComponentBase.h"
#include"../../RenderSystem/JGDeviceD.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGBufferManager.h"
#include"../../RenderSystem/JGViewportD.h"
#include"../../RenderSystem/JGRenderSuperClass.h"
JGRenderSuperClass* ComponentBase::m_RenderSuperClass;
ComponentBase::ComponentBase()
{
}

ComponentBase::~ComponentBase()
{
}

void ComponentBase::InitComponent(JGRenderSuperClass* SuperClass)
{
	m_RenderSuperClass = SuperClass;
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
	return m_RenderSuperClass->GetDevice();
}
JGHLSLShaderDevice* ComponentBase::GetHLSLDevice()
{
	return m_RenderSuperClass->GetHLSLDevice();
}
JGBufferManager* ComponentBase::GetBufferManager()
{
	return m_RenderSuperClass->GetBufferManager();
}

JGViewportD * ComponentBase::GetViewport()
{
	return m_RenderSuperClass->GetViewport();
}