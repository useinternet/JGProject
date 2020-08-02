#pragma once

#include "GEObject.h"

/*
Class : Scene

ex : 카메라 설정, 컬링 설정, 렌더러 설정, 씬 오브젝트 배치



*/

namespace GE
{
	class SceneCamera;
	class SceneObject;
	class Texture;
	class GRAPHICSENGINE_API Scene : public GEObject
	{
	public:
		virtual void BindCamera(SceneCamera* cam) = 0;
		virtual void UnBindCamera() = 0;

		virtual void PushObject(SceneObject* sceneObject) = 0;
		virtual void PushObjects(const std::vector<SceneObject*>& sceneObject) = 0;
		virtual void FlushObjects() = 0;

		virtual SceneCamera* GetBindedCamera() const = 0;
		virtual std::vector<SceneObject*> GetPushedObjects() const = 0;


		virtual Texture* GetSceneTexture() const = 0;

		virtual ~Scene() {}
		// 
	};



	class GRAPHICSENGINE_API SceneCamera
	{
	public:
		void SetLens(float fov, float width, float height, float farZ, float nearZ, bool is_orthographic = false) {

			m_CameraCB.lensWidth = width;
			m_CameraCB.lensHeight = height;
			m_CameraCB.farZ = farZ;
			m_CameraCB.nearZ = nearZ;
			m_FOV = fov;
			m_IsOrthographic = is_orthographic;


			m_IsDirty = true;
		}
		void SetPosition(const JVector3& pos) {
			m_Position = pos;
			m_IsDirty = true;
		}
		void SetRotation(const JVector3& rotate) {
			

			m_Rotation = rotate;
			CheckAngle();
			m_IsDirty = true;
		}
		void SetRotationPitch(float pitch) {
			m_Rotation.x = pitch;
			CheckAngle();
			m_IsDirty = true;
		}
		void SetRotationYaw(float yaw) {

			m_Rotation.y = yaw;
			CheckAngle();
			m_IsDirty = true;
		}
		void SetRotationRoll(float roll) {
			m_Rotation.z = roll;
			CheckAngle();
			m_IsDirty = true;
		}

		void OffsetPosition(const JVector3& off)
		{
			m_Position += off;
			CheckAngle();
			m_IsDirty = true;
		}
		void OffsetRotation(const JVector3& rotate) {
			m_Rotation += rotate;
			CheckAngle();
			m_IsDirty = true;
		}
		void OffsetRotationPitch(float pitch) {
			m_Rotation.x += pitch;
			CheckAngle();
			m_IsDirty = true;
		}
		void OffsetRotationYaw(float yaw) {
			m_Rotation.y += yaw;
			CheckAngle();
			m_IsDirty = true;
		}
		void OffsetRotationRoll(float roll) {
			m_Rotation.z += roll;
			CheckAngle();
			m_IsDirty = true;
		}

		const JVector3& GetRotation() const {
			return m_Rotation;
		}
		const JVector3& GetPosition() const {
			return m_Position;
		}

		const JMatrix& GetView() const {
			Flush();
			return m_CameraCB.view;
		}

		const JMatrix& GetProj() const {
			Flush();
			return m_CameraCB.proj;
		}

		const JMatrix& GetViewProj() const {
			Flush();
			return m_CameraCB.viewProj;
		}
		const CBCamera& GetCameraCB() const {
			Flush();
			return m_CameraCB;
		}
	private:
		void Flush() const
		{
			if (!m_IsDirty) return;

			if (m_IsOrthographic)
			{
				m_CameraCB.proj = JMatrix::OrthographicLH(m_CameraCB.lensWidth, m_CameraCB.lensHeight, m_CameraCB.nearZ, m_CameraCB.farZ);
			}
			else
			{
				m_CameraCB.proj = JMatrix::PerspectiveFovLH(m_FOV, m_CameraCB.lensWidth / m_CameraCB.lensHeight, m_CameraCB.nearZ, m_CameraCB.farZ);
			}

			JMatrix rotationMatrix = JMatrix::Rotation(JQuaternion::ToQuaternion(m_Rotation));

			JVector3 target = { 0.0f,0.0f,1.0f };
			JVector3 up     = { 0.0f,1.0f,0.0f };

			JVector3 lookVec = target - m_Position;
			//lookVec = JVector3::Normalize(lookVec);
			lookVec = rotationMatrix.TransformVector(lookVec);
			up = rotationMatrix.TransformVector(up);

			m_CameraCB.view = JMatrix::LookAtLH(m_Position, lookVec, up);
			m_CameraCB.viewProj = m_CameraCB.view * m_CameraCB.proj;
			m_CameraCB.invView = JMatrix::Inverse(m_CameraCB.view);
			m_CameraCB.invProj = JMatrix::Inverse(m_CameraCB.proj);
			m_CameraCB.invViewProj = JMatrix::Inverse(m_CameraCB.viewProj);


			m_CameraCB.view = JMatrix::Transpose(m_CameraCB.view);
			m_CameraCB.proj = JMatrix::Transpose(m_CameraCB.proj);
			m_CameraCB.viewProj = JMatrix::Transpose(m_CameraCB.viewProj);
			m_CameraCB.invView = JMatrix::Transpose(m_CameraCB.invView);
			m_CameraCB.invProj = JMatrix::Transpose(m_CameraCB.invProj);
			m_CameraCB.invViewProj = JMatrix::Transpose(m_CameraCB.invViewProj);

			m_IsDirty = false;
		}
		void CheckAngle()
		{
			if (std::abs(m_Rotation.x) >= JG_2PI) m_Rotation.x = 0.0f;
			if (std::abs(m_Rotation.y) >= JG_2PI) m_Rotation.y = 0.0f;
			if (std::abs(m_Rotation.z) >= JG_2PI) m_Rotation.z = 0.0f;

			if (m_Rotation.x < 0.0f) m_Rotation.x = JG_2PI - m_Rotation.x;
			if (m_Rotation.y < 0.0f) m_Rotation.y = JG_2PI - m_Rotation.y;
			if (m_Rotation.z < 0.0f) m_Rotation.z = JG_2PI - m_Rotation.z;
		}
	private:
		JVector3 m_Position;
		JVector3 m_Rotation;


		mutable CBCamera m_CameraCB;
		float m_FOV = 0.0F;

		bool m_IsOrthographic = false;
		mutable bool m_IsDirty = true;
	};
}
