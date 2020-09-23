#pragma once
#include "GEObject.h"
namespace GE
{
	class GRAPHICSENGINE_API SceneCamera
	{
	public:
		SceneCamera();
		void SetLens(float fov, float width, float height, float farZ, float nearZ, bool is_orthographic = false);
		void SetPosition(const JVector3& pos);
		void SetRotation(const JQuaternion& q);

		const JQuaternion& GetRotation() const;
		const JVector3& GetPosition() const;
		const CBCamera& GetCameraCB() const;

		void MoveForward(float f);
		void MoveRight(float f);

		const JVector3& GetLook()  const {
			return m_Look;
		}
		const JVector3& GetUp()    const {
			return m_Up;
		}
		const JVector3& GetRight() const {
			return m_Right;
		}
		JVector3 ScreenToWorldToPoint(const JVector3& p);
		JVector3 ScreenToWorldToRay(const JVector3& p);
		float GetFOV() const {
			return m_FOV;
		}
		const JFrustum& GetFrustum() const {
			Flush();
			return m_Frustum;
		}
	private:
		void Flush() const;
	private:
		JVector3    m_Position;
		JQuaternion m_Rotation;
		mutable JVector3 m_Look;
		mutable JVector3 m_Up;
		mutable JVector3 m_Right;
		mutable JFrustum m_Frustum;
		mutable CBCamera m_CameraCB;
		float m_FOV = 0.0F;

		bool m_IsOrthographic = false;
		mutable bool m_IsDirty = true;
	};
}
