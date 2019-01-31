#pragma once
#include"DxCommon/DxCommon.h"



namespace JGRC
{
	class JGLight;
	class RCORE_EXPORT Interface_SceneDirectionLight
	{
		friend class Interface_Scene;
	private:
		JGLight* m_Data = nullptr;
	public:
		Interface_SceneDirectionLight() = default;
		Interface_SceneDirectionLight(JGLight* l);
		Interface_SceneDirectionLight* operator->()
		{
			return this;
		}
	public:
		void SetLightColor(float r, float g, float b);
		void SetDirection(float x, float y, float z);
		void SetRotation(float pitch, float yaw, float roll);
	public:
		void OffsetLightColor(float r, float g, float b);
		void OffsetDirection(float x, float y, float z);
		void OffsetRotation(float pitch, float yaw, float roll);
	};

	typedef Interface_SceneDirectionLight IF_DirectionLight;

}