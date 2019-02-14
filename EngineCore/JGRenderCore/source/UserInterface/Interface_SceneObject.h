#pragma once
#include"DxCommon/DxCommon.h"


namespace JGRC
{
	class Interface_Scene;
	enum class RCORE_EXPORT E_IF_ObjType
	{
		Static,
		Dynamic,
	};
	class RCORE_EXPORT Interface_SceneObject
	{
		friend Interface_Scene;
	private:
		class SceneObject* m_Data = nullptr;


	public:
		Interface_SceneObject* operator->()
		{
			return this;
		}
	public:
		Interface_SceneObject() = default;
		Interface_SceneObject(SceneObject* obj);
		Interface_SceneObject(const Interface_SceneObject& copy)
		{
			m_Data = copy.m_Data;
		}
	public:
		DirectX::XMFLOAT3 GetLocation() const;
		DirectX::XMFLOAT3 GetRotation() const;
		DirectX::XMFLOAT3 GetScale()    const;
	public:
		void SetLocation(float x, float y, float z);
		void SetLocation(const DirectX::XMFLOAT3& v);
		void SetRotation(float pitch, float yaw, float roll);
		void SetRotation(const DirectX::XMFLOAT3& r);
		void SetScale(float x, float y, float z); 
		void SetScale(const DirectX::XMFLOAT3& v);
		void SetScale(float x);
		void SetAnimation(const std::string& name);
	public:
		void OffsetLocation(float x, float y, float z);
		void OffsetLocation(const DirectX::XMFLOAT3& v);
		void OffsetRotation(float pitch, float yaw, float roll); 
		void OffsetRotation(const DirectX::XMFLOAT3& v);
		void OffsetScale(float x, float y, float z); 
		void OffsetScale(const DirectX::XMFLOAT3& v);
		void OffsetScale(float x); 

	public:
		void AttachTo(Interface_SceneObject obj);
		void Visible();
		void Active();
		
		void UnVisible();
		void DeActive();

		bool IsVisible() const;
		bool IsActive()  const;

		E_IF_ObjType GetType() const;
	};

	typedef Interface_SceneObject IF_Object;
}