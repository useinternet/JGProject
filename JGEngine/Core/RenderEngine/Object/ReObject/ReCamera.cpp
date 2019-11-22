#include "pch.h"
#include "ReCamera.h"
#include "Object/Shader/ShaderDataType.h"

using namespace std;


namespace RE
{
	ReCamera::ReCamera(const std::string& name) : ReObject(name)
	{

	}

	void ReCamera::SetLens(float fov_angle, float lens_width, float lens_height,
		float nearZ, float farZ, bool is_orthographic)
	{
		m_Is_Orthographic = is_orthographic;
		m_Width  = lens_width;
		m_Height = lens_height;
		m_FOV    = fov_angle;
		m_NearZ  = nearZ;
		m_FarZ   = farZ;


		m_isProjUpdate = true;
	}
	const JMatrix& ReCamera::GetView()
	{
		if (m_isViewUpdate)
		{
			JVector3 up = { 0.0f,1.0f,0.0 };
			JVector3 target = GetLook();
	
			target += m_Position;

			m_View = JMatrix::LookAtLH(m_Position, target, up);
			m_isViewUpdate = false;
			m_isViewProjUpdate = true;
		}

		return m_View;
	}
	const JMatrix& ReCamera::GetProj()
	{
		if (m_isProjUpdate)
		{
			float radian_fov = DirectX::XMConvertToRadians(m_FOV);
			float ratio = (float)m_Width / (float)m_Height;

			if (!m_Is_Orthographic)
			{
				m_Proj = JMatrix::PerspectiveFovLH(radian_fov, ratio, m_NearZ, m_FarZ);
			}
			else
			{
				m_Proj = JMatrix::OrthographicLH(m_Width, m_Height, m_NearZ, m_FarZ);
			}

			m_isProjUpdate = false;
			m_isViewProjUpdate = true;
		}

		return m_Proj;
	}
	const JMatrix& ReCamera::GetViewProj()
	{
		if (m_isViewProjUpdate)
		{
			m_ViewProj = GetView() * GetProj();
			m_isViewProjUpdate = false;
		}
		return m_ViewProj;
	}

	JVector3 ReCamera::GetRight() const {

		JVector3 up = GetUp();
		JVector3 look = GetLook();

		return JVector3::Normalize(JVector3::Cross(look, up));
	}
	JVector3 ReCamera::GetUp()    const {
		JVector3 up;
		auto m = JMatrix::Rotation(JQuaternion::ToQuaternion(m_Rotation));
		up = JVector3::Normalize(m.TransformVector(up));
		return up;
	}
	JVector3 ReCamera::GetLook()  const {
		JVector3 look = { 0.0f,0.0f,1.0f };
		auto m = JMatrix::Rotation(JQuaternion::ToQuaternion(m_Rotation));
		look = JVector3::Normalize(m.TransformVector(look));
		return look;
	}
}