#pragma once
#include"Data/JGRCObject.h"


namespace JGRC
{
	class RCORE_EXPORT Interface_SceneObject
	{
	private:
		JGRCObject* m_ObjectData = nullptr;


	public:
		Interface_SceneObject* operator->()
		{
			return this;
		}
	public:
		Interface_SceneObject(JGRCObject* obj) : m_ObjectData(obj) {}
		Interface_SceneObject(const Interface_SceneObject& copy)
		{
			m_ObjectData = copy.m_ObjectData;
		}
	public:
		Vec3 GetLocation() const { return m_ObjectData->GetLocation(); }
		Vec3 GetRotation() const { return m_ObjectData->GetRotation(); }
		Vec3 GetScale()    const { return m_ObjectData->GetScale(); }
	public:
		void SetLocation(float x, float y, float z)          { m_ObjectData->SetLocation(x, y, z); }
		void SetLocation(const Vec3& v)                      { m_ObjectData->SetLocation(v.x, v.y, v.z); }
		void SetRotation(float pitch, float yaw, float roll) { m_ObjectData->SetRotation(pitch, yaw, roll); }
		void SetRotation(const Vec3& r)                      { m_ObjectData->SetRotation(r.x, r.y, r.z); }
		void SetScale(float x, float y, float z)             { m_ObjectData->SetScale(x, y, z); }
		void SetScale(const Vec3& v)                         { m_ObjectData->SetScale(v.x, v.y, v.z); }
		void SetScale(float x)                               { m_ObjectData->SetScale(x); }
	public:
		void OffsetLocation(float x, float y, float z)          { m_ObjectData->OffsetLocation(x, y, z); }
		void OffsetLocation(const Vec3& v)                      { m_ObjectData->OffsetLocation(v.x, v.y, v.z); }
		void OffsetRotation(float pitch, float yaw, float roll) { m_ObjectData->OffsetRotation(pitch, yaw, roll); }
		void OffsetRotation(const Vec3& v)                      { m_ObjectData->OffsetRotation(v.x, v.y, v.z); }
		void OffsetScale(float x, float y, float z)             { m_ObjectData->SetScale(x, y, z); }
		void OffsetScale(const Vec3& v)                         { m_ObjectData->SetScale(v.x, v.y, v.z); }
		void OffsetScale(float x)                               { m_ObjectData->SetScale(x); }
	public:
		void Visible()   { m_ObjectData->Visible(); }
		void Active()    { m_ObjectData->Active(); }
		
		void UnVisible() { m_ObjectData->UnVisible(); }
		void DeActive()  { m_ObjectData->DeActive(); }

		bool IsVisible() const { return m_ObjectData->IsVisible(); }
		bool IsActive()  const { return m_ObjectData->IsActive(); }

		EObjType GetType() const { return m_ObjectData->GetType(); }
	};

	typedef Interface_SceneObject IF_Object;
}