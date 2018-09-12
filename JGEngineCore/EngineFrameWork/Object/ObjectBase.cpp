#include "ObjectBase.h"
#include"../../RenderSystem/JGDeviceD.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGBufferManager.h"
#include"../../RenderSystem/JGViewportD.h"
#include"../../EngineStatics/JGSuperClass.h"
#include"../World/World.h"
JGSuperClass* ObjectBase::m_RenderSuperClass = nullptr;;
ObjectBase::ObjectBase()
{

}
ObjectBase::~ObjectBase()
{
}


void ObjectBase::InitObejct(JGSuperClass* SuperClass)
{
	m_RenderSuperClass = SuperClass;
}
int ObjectBase::GetZOrder()
{
	return ZOrder;
}
void ObjectBase::SetZOrder(int Num)
{
	ZOrder = Num;
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

JGSuperClass * ObjectBase::GetRenderSuperClass()
{
	return m_RenderSuperClass;
}
