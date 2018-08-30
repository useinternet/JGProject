#include "ObjectBase.h"
#include"../../RenderSystem/JGDeviceD.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGBufferManager.h"
#include"../../RenderSystem/JGViewportD.h"
#include"../../RenderSystem/JGRenderSuperClass.h"
JGRenderSuperClass* ObjectBase::m_RenderSuperClass = nullptr;;
ObjectBase::ObjectBase()
{

}
ObjectBase::~ObjectBase()
{
}


void ObjectBase::InitObejct(JGRenderSuperClass* SuperClass)
{
	m_RenderSuperClass = SuperClass;
}

void ObjectBase::RegisterObjectID(const type_info& ID)
{
	ObjectID = ID.name();
}

const char* ObjectBase::GetID()
{
	return ObjectID;
}


JGDeviceD* ObjectBase::GetDevice()
{
	return m_RenderSuperClass->GetDevice();
}
JGHLSLShaderDevice* ObjectBase::GetHLSLDevice()
{
	return m_RenderSuperClass->GetHLSLDevice();
}
JGBufferManager* ObjectBase::GetBufferManager()
{
	return m_RenderSuperClass->GetBufferManager();
}

JGViewportD * ObjectBase::GetViewport()
{
	return m_RenderSuperClass->GetViewport();
}

JGRenderSuperClass * ObjectBase::GetRenderSuperClass()
{
	return m_RenderSuperClass;
}
