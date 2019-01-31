#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	class JGLight;
	class RCORE_EXPORT Interface_ScenePointLight
	{
		friend class Interface_Scene;
	private:
		JGLight* m_Data = nullptr;
	public:
		Interface_ScenePointLight() = default;
		Interface_ScenePointLight(JGLight* l);
		Interface_ScenePointLight* operator->()
		{
			return this;
		}
	public:
		void SetLightColor(float r, float g, float b);
		void SetLocation(float x, float y, float z);
		void SetFalloffStart(float x);
		void SetFalloffEnd(float x);
	public:
		void OffsetLightColor(float r, float g, float b);
		void OffsetLocation(float x, float y, float z);
		void OffsetFalloffStart(float x);
		void OffsetFalloffEnd(float x);
	};

	typedef Interface_ScenePointLight IF_PointLight;
}