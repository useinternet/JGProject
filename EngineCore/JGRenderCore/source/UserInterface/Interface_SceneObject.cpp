#include"Interface_SceneObject.h"
#include"Data/Object/SceneObject.h"
#include"Interface_Scene.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
using namespace JGLibrary;
Interface_SceneObject::Interface_SceneObject(SceneObject* obj) : m_Data(obj) {}

Location& Interface_SceneObject::GetLocation()
{
	return m_Data->GetLocation();
}
Rotation& Interface_SceneObject::GetRotation()
{
	return m_Data->GetRotation();
}
Scale&    Interface_SceneObject::GetScale()
{
	return m_Data->GetScale();
}
void Interface_SceneObject::SetAnimation(const std::string& name)
{
	m_Data->SetAnimation(name);
}
void Interface_SceneObject::AttachTo(Interface_SceneObject obj) { m_Data->AttachTo(obj.m_Data); }
void Interface_SceneObject::Visible() { m_Data->Visible(); }
void Interface_SceneObject::Active()  { m_Data->Active(); }

void Interface_SceneObject::UnVisible() { m_Data->UnVisible(); }
void Interface_SceneObject::DeActive()  { m_Data->DeActive(); }

bool Interface_SceneObject::IsVisible() const { return m_Data->IsVisible(); }
bool Interface_SceneObject::IsActive()  const { return m_Data->IsActive(); }

E_IF_ObjType Interface_SceneObject::GetType() const {
	switch (m_Data->GetType())
	{
	case EObjectType::Static:
		return E_IF_ObjType::Static;
	case EObjectType::Dynamic:
		return E_IF_ObjType::Dynamic;
	}
	return (E_IF_ObjType)-1;
} 