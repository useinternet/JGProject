#include"ComponentBase.h"
#include"../../RenderSystem/JGDeviceD.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGBufferManager.h"
#include"../../RenderSystem/JGViewportD.h"
#include"../../RenderSystem/JGRenderSuperClass.h"
const char* ComponentBase::ComponentID;
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
	static bool bInit = false;
	if (!bInit)
	{
		bInit = true;
		ComponentID = ID.name();
	}

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