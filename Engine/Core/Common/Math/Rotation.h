#pragma once
#include"Common.h"
#include"Vector.h"
#include"Matrix.h"

namespace Common
{
	class Transformation;
	class Rotation
	{
		friend Transformation;
	private:
		JVector3   m_Rotation;
		JMatrix4x4 m_RotationMatrix;
	public:
		Rotation() : m_Rotation(0.0f, 0.0f, 0.0f) {}
		Rotation(const Rotation& copy) = default;
		Rotation(Rotation&& copy) = default;
		Rotation& operator=(const Rotation& rhs) = default;
		Rotation& operator=(Rotation&& rhs) = default;
		Rotation(const JVector3& v) : m_Rotation(v) {}
	public:
		void Set(const JVector3& v) {
			m_Rotation = v;
			UpdateMatrix();
		}
		void Offset(const JVector3& v) {
			m_Rotation += v;
			UpdateMatrix();
		}
		void SetPitch(float x) {
			m_Rotation.SetX(x);
			UpdateMatrix();
		}
		void SetYaw(float y) {
			m_Rotation.SetY(y);
			UpdateMatrix();
		}
		void SetRoll(float z) {
			m_Rotation.SetZ(z);
			UpdateMatrix();
		}
		void OffsetPitch(float x) {
			m_Rotation.SetX(m_Rotation.X() + x);
			UpdateMatrix();
		}
		void OffsetYaw(float y) {
			m_Rotation.SetY(m_Rotation.Y() + y);
			UpdateMatrix();
		}
		void OffsetRoll(float z) {
			m_Rotation.SetZ(m_Rotation.Z() + z);
			UpdateMatrix();
		}
		const JMatrix4x4& GetMatrix() const {
			return m_RotationMatrix;
		}
		const JVector3& Get() const {
			return m_Rotation;
		}
		float GetPitch() const {
			return m_Rotation.X();
		}
		float GetYaw() const {
			return m_Rotation.Y();
		}
		float GetRoll() const {
			return m_Rotation.Z();
		}
	public:
		bool operator==(const Rotation& pos) const {
			return m_Rotation == pos.m_Rotation;
		}
		bool operator!=(const Rotation& pos) const {
			return m_Rotation != pos.m_Rotation;
		}
	public:
		Rotation operator+(const Rotation& pos) const {
			return Rotation(m_Rotation + pos.m_Rotation);
		}
		Rotation operator+(float k) const {
			JVector3 result = m_Rotation;
			result.AddScalar(k);
			return Rotation(result);
		}
		Rotation operator-(const Rotation& pos) const {
			return Rotation(m_Rotation - pos.m_Rotation);
		}
		Rotation operator-(float k) const {
			JVector3 result = m_Rotation;
			result.AddScalar(-k);
			return Rotation(result);
		}
	public:
		Rotation& operator+=(const Rotation& pos) {
			m_Rotation += pos.m_Rotation;
			UpdateMatrix();
			return *this;
		}
		Rotation& operator+=(float k) {
			m_Rotation.AddScalar(k);
			UpdateMatrix();
			return *this;
		}
		Rotation& operator-=(const Rotation& pos) {
			m_Rotation -= pos.m_Rotation;
			UpdateMatrix();
			return *this;
		}
		Rotation& operator-=(float k) {
			m_Rotation.AddScalar(-k);
			UpdateMatrix();
			return *this;
		}
	public:
		Rotation operator*(float k) const {
			return Rotation(m_Rotation * k);
		}
		Rotation operator/(float k) const {
			return Rotation(m_Rotation / k);
		}
		Rotation& operator*=(float k) {
			m_Rotation *= k;
			UpdateMatrix();
			return *this;
		}
		Rotation& operator/=(float k) {
			m_Rotation /= k;
			UpdateMatrix();
			return *this;
		}
	private:
		void UpdateMatrix()
		{
			if (m_Rotation.X() > 360.0f)
				m_Rotation.SetX(m_Rotation.X() - 360.0f);
			if (m_Rotation.Y() > 360.0f)
				m_Rotation.SetY(m_Rotation.Y() - 360.0f);
			if (m_Rotation.Z() > 360.0f)
				m_Rotation.SetZ(m_Rotation.Z() - 360.0f);

			m_RotationMatrix.Rotation(
				DirectX::XMConvertToRadians(m_Rotation.X()),
				DirectX::XMConvertToRadians(m_Rotation.Y()),
				DirectX::XMConvertToRadians(m_Rotation.Z()));
		}
	};
}