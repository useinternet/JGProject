#include"Interface_SceneObject.h"
#include"Data/Object/SceneObject.h"
#include"Interface_Scene.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
Interface_SceneObject::Interface_SceneObject(SceneObject* obj) : m_Data(obj) {}
XMFLOAT3 Interface_SceneObject::GetLocation() const { return m_Data->GetLocation(); }
XMFLOAT3 Interface_SceneObject::GetRotation() const { return m_Data->GetRotation(); }
XMFLOAT3 Interface_SceneObject::GetScale()    const { return m_Data->GetScale(); }

void Interface_SceneObject::SetLocation(float x, float y, float z)          { m_Data->SetLocation(x, y, z); }
void Interface_SceneObject::SetLocation(const XMFLOAT3& v)                  { m_Data->SetLocation(v.x, v.y, v.z); }
void Interface_SceneObject::SetRotation(float pitch, float yaw, float roll) { m_Data->SetRotation(pitch, yaw, roll); }
void Interface_SceneObject::SetRotation(const XMFLOAT3& r)                  { m_Data->SetRotation(r.x, r.y, r.z); }
void Interface_SceneObject::SetScale(float x, float y, float z)             { m_Data->SetScale(x, y, z); }
void Interface_SceneObject::SetScale(const XMFLOAT3& v)                     { m_Data->SetScale(v.x, v.y, v.z); }
void Interface_SceneObject::SetScale(float x)                               { m_Data->SetScale(x); }
void Interface_SceneObject::SetAnimation(const string& name)                { m_Data->SetAnimation(name); }
void Interface_SceneObject::OffsetLocation(float x, float y, float z)          { m_Data->OffsetLocation(x, y, z); }
void Interface_SceneObject::OffsetLocation(const XMFLOAT3& v)                  { m_Data->OffsetLocation(v.x, v.y, v.z); }
void Interface_SceneObject::OffsetRotation(float pitch, float yaw, float roll) { m_Data->OffsetRotation(pitch, yaw, roll); }
void Interface_SceneObject::OffsetRotation(const XMFLOAT3& v)                  { m_Data->OffsetRotation(v.x, v.y, v.z); }
void Interface_SceneObject::OffsetScale(float x, float y, float z)             { m_Data->SetScale(x, y, z); }
void Interface_SceneObject::OffsetScale(const XMFLOAT3& v)                     { m_Data->SetScale(v.x, v.y, v.z); }
void Interface_SceneObject::OffsetScale(float x)                               { m_Data->SetScale(x); }
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