#pragma once


#include "Object/ReObject.h"

namespace RE
{
	class STStruct;
	class ReCamera : public ReObject
	{
	private:
		JVector3 m_Position;
		JVector3 m_Rotation;
		JMatrix  m_View = JMatrix::Identity();
		JMatrix  m_Proj = JMatrix::Identity();
		JMatrix  m_ViewProj = JMatrix::Identity();
		bool  m_Is_Orthographic = false;
		float m_Width  = 0.0f;
		float m_Height = 0.0f;
		float m_FOV    = 0.0f;
		float m_NearZ  = 0.0f;
		float m_FarZ   = 0.0f;


		bool m_isViewUpdate = true;
		bool m_isProjUpdate = true;
		bool m_isViewProjUpdate = true;
	public:
		ReCamera(const std::string& name = "ReCamera");

		void SetLens(float fov_angle, float lens_width, float lens_height,
			float nearZ = 0.1f , float farZ = 1000000, bool is_orthographic = false);

		float GetFOV() {
			return m_FOV;
		}
		void  SetFOV(float angle) {
			m_FOV = angle;
		}
		float GetLensWidth() const {
			return m_Width;
		}
		float GetLensHeight() const {
			return m_Height;
		}
		void  SetLensWidth(float width) {
			m_Width = width;
		}
		void  SetLensHeight(float height) {
			m_Height = height;
		}

		float GetNearZ() const {
			return m_NearZ;
		}
		float GetFarZ() const {
			return m_FarZ;
		}
		void  SetNearZ(float nearZ) {
			m_NearZ = nearZ;
		}
		void  SetFarZ(float farZ) {
			m_FarZ = farZ;
		}

		bool  IsOrthographic() const {
			return m_Is_Orthographic;
		}
		void  ConvertOrthographic()
		{
			m_Is_Orthographic = true;
		}
		void  ConvertPerspective() {
			m_Is_Orthographic = false;
		}







		const JMatrix& GetView();
		const JMatrix& GetProj();
		const JMatrix& GetViewProj();
		JVector3 GetRight() const;
		JVector3 GetUp()    const;
		JVector3 GetLook()  const;
		void Rotate(float pitch, float yaw, float roll) {
			m_Rotation = { pitch, yaw, roll };
			m_isViewUpdate = true;
		}
		void Rotate(const JVector3& rotate) {
			m_Rotation = rotate;
			m_isViewUpdate = true;
		}
		void RotatePitch(float pitch) {
			m_Rotation.x = pitch;
			m_isViewUpdate = true;
		}
		void RotateYaw(float yaw) {
			m_Rotation.y = yaw;
			m_isViewUpdate = true;
		}
		void RotateRoll(float roll) {
			m_Rotation.z = roll;
			m_isViewUpdate = true;
		}
		void AddRotate(float pitch, float yaw, float roll) {
			m_Rotation += {pitch, yaw, roll};
			m_isViewUpdate = true;
		}
		void AddRotate(const JVector3& rotate) {
			m_Rotation += rotate;
			m_isViewUpdate = true;
		}
		void AddRotatePitch(float pitch) {
			m_Rotation.x += pitch;
			m_isViewUpdate = true;
		}
		void AddRotateYaw(float yaw) {
			m_Rotation.y += yaw;
			m_isViewUpdate = true;
		}
		void AddRotateroll(float roll) {
			m_Rotation.z += roll;
			m_isViewUpdate = true;
		}












		const JVector3& GetPosition() const {
			return m_Position;
		}
		void SetPosition(float x, float y, float z) {
			m_Position = { x,y,z };
			m_isViewUpdate = true;
		}
		void SetPosition(const JVector3& v) {
			m_Position = v;
			m_isViewUpdate = true;
		}
		void SetPositionX(float x) {
			m_Position.x = x;
			m_isViewUpdate = true;
		}
		void SetPositionY(float y) {
			m_Position.y = y;
			m_isViewUpdate = true;
		}
		void SetPositionZ(float z) {
			m_Position.z = z;
			m_isViewUpdate = true;
		}
		void AddPosition(float x, float y, float z) {
			m_Position += {x, y, z};
			m_isViewUpdate = true;
		}
		void AddPosition(const JVector3& v) {
			m_Position += v;
			m_isViewUpdate = true;
		}
		void AddPositionX(float x) {
			m_Position.x += x;
			m_isViewUpdate = true;
		}
		void AddPositionY(float y) {
			m_Position.y += y;
			m_isViewUpdate = true;
		}
		void AddPositionZ(float z) {
			m_Position.z += z;
			m_isViewUpdate = true;
		}
	};
}