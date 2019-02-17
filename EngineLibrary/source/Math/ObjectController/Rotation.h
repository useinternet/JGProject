#pragma once
#include"Math/GeoMath.h"
#include"Math/Transform.h"


namespace JGLibrary
{
	class Rotation
	{
	private:
		JPoint3   m_Rotation;
		Transform m_Transform;
		bool      m_bUpdate = true;
	public:
		Rotation()  = default;
		~Rotation() = default;
		Rotation(float pitch, float yaw, float roll) : m_Rotation(pitch, yaw, roll) {}
		Rotation(const JPoint3& p)  : m_Rotation(p) {}
		Rotation(const JVector3& v) : m_Rotation(v) {}
		Rotation& operator=(const Rotation& rhs) {
			m_Rotation = rhs.m_Rotation;
			m_Transform = rhs.m_Transform;
		}
	public:
		bool IsUpdate() {
			return m_bUpdate;
		}
		void Set(float pitch, float yaw, float roll) {
			m_bUpdate = true;
			m_Rotation = JPoint3(pitch, yaw, roll);
		}
		void Set(const JPoint3& p) {
			m_bUpdate = true;
			m_Rotation = p;
		}
		void Set(const JVector3& v) {
			m_bUpdate = true;
			m_Rotation = JPoint3(v);
		}
		void SetPitch(float pitch) {
			m_bUpdate = true;
			m_Rotation.mData.x = pitch;
		}
		void SetYaw(float yaw) {
			m_bUpdate = true;
			m_Rotation.mData.y = yaw;
		}
		void SetRoll(float roll) {
			m_bUpdate = true;
			m_Rotation.mData.z = roll;
		}
		void Offset(float pitch, float yaw, float roll) {
			m_bUpdate = true;
			m_Rotation += JPoint3(pitch, yaw, roll);
		}
		void Offset(const JPoint3& p) {
			m_bUpdate = true;
			m_Rotation += p;
		}
		void Offset(const JVector3& v) {
			m_bUpdate = true;
			m_Rotation += v;
		}
		void OffsetPitch(float pitch) {
			m_bUpdate = true;
			m_Rotation.mData.x += pitch;
		}
		void OffsetYaw(float yaw) {
			m_bUpdate = true;
			m_Rotation.mData.y += yaw;
		}
		void OffsetRoll(float roll) {
			m_bUpdate = true;
			m_Rotation.mData.z += roll;
		}
		const JPoint3& Get_c() const {
			return m_Rotation;
		}
		JPoint3& Get() {
			return m_Rotation;
		}
		Transform& GetTransform() {
			if (m_bUpdate)
			{
				m_Transform.Identity();
				m_Transform.Rotation(m_Rotation);
				m_bUpdate = false;
			}
			return m_Transform;
		}
		float Pitch() const {
			return m_Rotation.mData.x;
		}
		float Yaw() const {
			return m_Rotation.mData.y;
		}
		float Roll() const {
			return m_Rotation.mData.z;
		}
		const xmfloat3& Data_3f() const {
			return m_Rotation.mData;
		}
		const xmfloat4x4& Data_4x4f() {
			if (m_bUpdate)
			{
				m_Transform.Identity();
				m_Transform.Rotation(m_Rotation);
				m_bUpdate = false;
			}
			return m_Transform.mData;
		}
	};
}