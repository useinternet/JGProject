#include"Interface_SceneObject.h"
#include"Data/JGRCObject.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;
Interface_SceneObject::Interface_SceneObject(JGRCObject* obj) : m_ObjectData(obj) {}
XMFLOAT3 Interface_SceneObject::GetLocation() const { return m_ObjectData->GetLocation(); }
XMFLOAT3 Interface_SceneObject::GetRotation() const { return m_ObjectData->GetRotation(); }
XMFLOAT3 Interface_SceneObject::GetScale()    const { return m_ObjectData->GetScale(); }

void Interface_SceneObject::SetLocation(float x, float y, float z)          { m_ObjectData->SetLocation(x, y, z); }
void Interface_SceneObject::SetLocation(const XMFLOAT3& v)                  { m_ObjectData->SetLocation(v.x, v.y, v.z); }
void Interface_SceneObject::SetRotation(float pitch, float yaw, float roll) { m_ObjectData->SetRotation(pitch, yaw, roll); }
void Interface_SceneObject::SetRotation(const XMFLOAT3& r)                  { m_ObjectData->SetRotation(r.x, r.y, r.z); }
void Interface_SceneObject::SetScale(float x, float y, float z)             { m_ObjectData->SetScale(x, y, z); }
void Interface_SceneObject::SetScale(const XMFLOAT3& v)                     { m_ObjectData->SetScale(v.x, v.y, v.z); }
void Interface_SceneObject::SetScale(float x)                               { m_ObjectData->SetScale(x); }
void Interface_SceneObject::SetAnimation(const string& name)                { m_ObjectData->SetAnimation(name); }
void Interface_SceneObject::OffsetLocation(float x, float y, float z)          { m_ObjectData->OffsetLocation(x, y, z); }
void Interface_SceneObject::OffsetLocation(const XMFLOAT3& v)                  { m_ObjectData->OffsetLocation(v.x, v.y, v.z); }
void Interface_SceneObject::OffsetRotation(float pitch, float yaw, float roll) { m_ObjectData->OffsetRotation(pitch, yaw, roll); }
void Interface_SceneObject::OffsetRotation(const XMFLOAT3& v)                  { m_ObjectData->OffsetRotation(v.x, v.y, v.z); }
void Interface_SceneObject::OffsetScale(float x, float y, float z)             { m_ObjectData->SetScale(x, y, z); }
void Interface_SceneObject::OffsetScale(const XMFLOAT3& v)                     { m_ObjectData->SetScale(v.x, v.y, v.z); }
void Interface_SceneObject::OffsetScale(float x)                               { m_ObjectData->SetScale(x); }

void Interface_SceneObject::Visible() { m_ObjectData->Visible(); }
void Interface_SceneObject::Active()  { m_ObjectData->Active(); }

void Interface_SceneObject::UnVisible() { m_ObjectData->UnVisible(); }
void Interface_SceneObject::DeActive()  { m_ObjectData->DeActive(); }

bool Interface_SceneObject::IsVisible() const { return m_ObjectData->IsVisible(); }
bool Interface_SceneObject::IsActive()  const { return m_ObjectData->IsActive(); }

E_IF_ObjType Interface_SceneObject::GetType() const {
	switch (m_ObjectData->GetType())
	{
	case EObjType::Static:
	case EObjType::Instance:
		return E_IF_ObjType::Static;
	case EObjType::Dynamic:
		return E_IF_ObjType::Dynamic;
	}
	return (E_IF_ObjType)-1;
} 