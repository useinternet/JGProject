#pragma once
#pragma once
#include"Math/GeoMath.h"
#include"Math/Transform.h"

namespace JGLibrary
{
	class Scale
	{
	private:
		JPoint3   m_Scale = JPoint3(1.0f,1.0f,1.0f);
		Transform m_Transform;
		bool      m_bUpdate = true;
	public:
		Scale() = default;
		~Scale() = default;
		Scale(float x, float y, float z) : m_Scale(x, y, z) {}
		Scale(float x) : m_Scale(x, x, x) {}
		Scale(const JPoint3& p)  : m_Scale(p) {}
		Scale(const JVector3& v) : m_Scale(v) {}
	public:
		Scale& operator=(const Scale& rhs) {
			m_Scale     = rhs.m_Scale;
			m_Transform = rhs.m_Transform;
		}
		bool IsUpdate() {
			return m_bUpdate;
		}
		void Set(float x, float y, float z) {
			m_bUpdate = true;
			m_Scale = JPoint3(x, y, z);
		}
		void Set(float x)
		{
			m_bUpdate = true;
			m_Scale = JPoint3(x, x, x);
		}
		void Set(const JPoint3& p) {
			m_bUpdate = true;
			m_Scale = p;
		}
		void Set(const JVector3& v) {
			m_bUpdate = true;
			m_Scale = JPoint3(v);
		}
		void SetX(float x) {
			m_bUpdate = true;
			m_Scale.mData.x = x;
		}
		void SetY(float y) {
			m_bUpdate = true;
			m_Scale.mData.y = y;
		}
		void SetZ(float z) {
			m_bUpdate = true;
			m_Scale.mData.z = z;
		}
		void Offset(float x, float y, float z) {
			m_bUpdate = true;
			m_Scale += JPoint3(x, y, z);
		}
		void Offset(float x) {
			m_bUpdate = true;
			m_Scale += JPoint3(x, x, x);
		}
		void Offset(const JPoint3& p) {
			m_bUpdate = true;
			m_Scale += p;
		}
		void Offset(const JVector3& v) {
			m_bUpdate = true;
			m_Scale += v;
		}
		void OffsetX(float x) {
			m_bUpdate = true;
			m_Scale.mData.x += x;
		}
		void OffsetY(float y) {
			m_bUpdate = true;
			m_Scale.mData.y += y;
		}
		void OffsetZ(float z) {
			m_bUpdate = true;
			m_Scale.mData.z += z;
		}
		const JPoint3& Get_c() const {
			return m_Scale;
		}
		JPoint3& Get() {
			return m_Scale;
		}
		Transform& GetTransform() {
			if (m_bUpdate)
			{
				m_Transform.Identity();
				m_Transform.Scaling(m_Scale);
				m_bUpdate = false;
			}
			return m_Transform;
		}
		float X() const {
			return m_Scale.mData.x;
		}
		float Y() const {
			return m_Scale.mData.y;
		}
		float Z() const {
			return m_Scale.mData.z;
		}
		const xmfloat3& Data_3f() const {
			return m_Scale.mData;
		}
		const xmfloat4x4& Data_4x4f() {
			if (m_bUpdate)
			{
				m_Transform.Identity();
				m_Transform.Scaling(m_Scale);
				m_bUpdate = false;
			}
			return m_Transform.mData;
		}
	};
}
