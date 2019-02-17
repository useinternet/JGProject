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
		JGLibrary::Location& GetLocation();
		JGLibrary::Rotation& GetRotation();
		JGLibrary::Scale&    GetScale();
		void SetAnimation(const std::string& name);
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