#include "pch.h"
#include "SceneCamera.h"

namespace GE
{
	SceneCamera::SceneCamera()
	{
		m_Look  = JVector3::Normalize(JVector3(0, 0, 1) - m_Position);
		m_Right = JVector3::Normalize(JVector3::Cross({ 0,1,0 }, m_Look));
		m_Up    = JVector3::Normalize(JVector3::Cross(m_Look, m_Right));
	}
	void SceneCamera::SetLens(float fov, float width, float height, float farZ, float nearZ, bool is_orthographic )
	{
		m_FOV = fov;
		m_IsOrthographic      = is_orthographic;
		m_CameraCB.nearZ      = nearZ;
		m_CameraCB.farZ       = farZ;
		m_CameraCB.lensWidth  = width;
		m_CameraCB.lensHeight = height;
		if (is_orthographic)
		{
			m_CameraCB.proj = JMatrix::Transpose(JMatrix::OrthographicLH(width, height, nearZ, farZ));
		}
		else
		{
			m_CameraCB.proj = JMatrix::Transpose(JMatrix::PerspectiveFovLH(fov, width / height, nearZ, farZ));
		}
		
		m_CameraCB.invProj = JMatrix::Inverse(m_CameraCB.proj);
		m_IsDirty = true;

	}
	void SceneCamera::SetPosition(const JVector3& pos)
	{
		m_Position = pos;
		m_IsDirty = true;
	}
	void SceneCamera::SetRotation(const JQuaternion& q)
	{
		m_Rotation = q;

		JMatrix r = JMatrix::Rotation(m_Rotation);
		m_Up    = r.TransformVector({ 0.0f,1.0f,0.0f });
		m_Look  = r.TransformVector({ 0.0f,0.0f,1.0f });
		m_Right = r.TransformVector({ 1.0f,0.0f,0.0f });
		m_IsDirty = true;
	}
	void SceneCamera::MoveForward(float f)
	{
		m_Position += (f * m_Look);
		m_IsDirty = true;
	}
	void SceneCamera::MoveRight(float f)
	{
		m_Position += (f * m_Right);
		m_IsDirty = true;
	}
	const JQuaternion& SceneCamera::GetRotation() const
	{
		return m_Rotation;
	}
	const JVector3& SceneCamera::GetPosition() const
	{
		return m_Position;
	}

	const CBCamera& SceneCamera::GetCameraCB() const
	{
		Flush();
		return m_CameraCB;
	}
	JVector3 SceneCamera::ScreenToWorldToPoint(const JVector3& p)
	{
		Flush();
		float farZ, nearZ;
		farZ = m_CameraCB.farZ;
		nearZ = m_CameraCB.nearZ;


		float z = (farZ + nearZ) / (2 * (farZ - nearZ)) + (1.0f / std::max<float>(p.z, nearZ)) * ((-farZ * nearZ) / (farZ - nearZ)) + 0.5f;
		


		JVector3 result = JVector3::UnProject(JVector3(p.x, p.y, z),
			JMatrix::Transpose(m_CameraCB.proj), JMatrix::Transpose(m_CameraCB.view),
			JMatrix::Identity(), JVector2(0, 0), JVector2(m_CameraCB.lensWidth, m_CameraCB.lensHeight));




		return result;
	}
	JVector3 SceneCamera::ScreenToWorldToRay(const JVector3& p)
	{
		JVector3 result = JVector3::UnProject(JVector3(p.x, p.y, 1.0f),
			JMatrix::Transpose(m_CameraCB.proj), JMatrix::Transpose(m_CameraCB.view),
			JMatrix::Identity(), JVector2(0, 0), JVector2(m_CameraCB.lensWidth, m_CameraCB.lensHeight));

		return JVector3::Normalize(result);

	}
	void SceneCamera::Flush() const
	{
		if (!m_IsDirty) return;
		m_IsDirty = false;

		m_CameraCB.view = JMatrix::Transpose(JMatrix::LookToLH(m_Position, m_Up, m_Right, m_Look));
		m_CameraCB.invView     = JMatrix::Inverse(m_CameraCB.view);
		m_CameraCB.viewProj    = m_CameraCB.proj * m_CameraCB.view;
		m_CameraCB.invViewProj = JMatrix::Inverse(m_CameraCB.viewProj);
		m_CameraCB.eyePos = m_Position;
		m_Frustum = JFrustum(JMatrix::Transpose(m_CameraCB.invViewProj));
	}
}