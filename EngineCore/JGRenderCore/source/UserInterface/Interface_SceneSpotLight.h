#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	class JGLight;
	class RCORE_EXPORT Interface_SceneSpotLight
	{
		friend class Interface_Scene;
	private:
		JGLight* m_Data = nullptr;
	public:
		Interface_SceneSpotLight() = default;
		Interface_SceneSpotLight(JGLight* l);
		Interface_SceneSpotLight* operator->()
		{
			return this;
		}
	public:
		void SetLightColor(float r, float g, float b);
		void SetDirection(float x, float y, float z);
		void SetLocation(float x, float y, float z);
		void SetFalloffStart(float x);
		void SetFalloffEnd(float x);
		void SetOuterAngle(float angle);
		void SetInnerAngle(float angle);
		void SetRotation(float pitch , float yaw, float roll);
	public:
		void OffsetLightColor(float r, float g, float b);
		void OffsetDirection(float x, float y, float z);
		void OffsetLocation(float x, float y, float z);
		void OffsetFalloffStart(float x);
		void OffsetFalloffEnd(float x);
		void OffsetOuterAngle(float angle);
		void OffsetInnerAngle(float angle);
		void OffsetRotation(float pitch, float yaw, float roll);
	};

	typedef Interface_SceneSpotLight IF_SpotLight;
}