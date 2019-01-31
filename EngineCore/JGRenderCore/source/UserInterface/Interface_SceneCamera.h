#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	class RCORE_EXPORT Interface_SceneCamera
	{
		friend class Interface_Scene;
	private:
		Camera* m_Data = nullptr;
	public:
		Interface_SceneCamera() = default;
		Interface_SceneCamera(Camera* cam) : m_Data(cam) {}
	public:
		Interface_SceneCamera* operator->()
		{
			return this;
		}
	public:
		void SetLens(float fov, float aspect, float nearz ,float farz);
		void SetPosition(float x, float y, float z);
		void Strafe(float d);
		void Walk(float d);
		void Up(float d);
		// Rotate the camera.
		void Pitch(float angle);
		void RotateY(float angle);
		void Roll(float angle);

	};
	typedef Interface_SceneCamera IF_Camera;
}