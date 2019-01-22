#pragma once
#include"Interface_SceneObject.h"

namespace JGRC
{
	class RCORE_EXPORT Interface_SceneInstanceObject
	{
	private:
		class InstanceObject* m_InsData = nullptr;
	public:
		Interface_SceneInstanceObject* operator->()
		{
			return this;
		}
	public:
		Interface_SceneInstanceObject() = default;
		Interface_SceneInstanceObject(class InstanceObject* obj) : m_InsData(obj) {}
		Interface_SceneInstanceObject(const Interface_SceneInstanceObject& copy)
		{
			m_InsData = copy.m_InsData;
		}
	public:
	//	IF_Object* AddObject() { return m_InsData->AddObject(); }
	};





	typedef Interface_SceneInstanceObject IF_InstanceObject;
}