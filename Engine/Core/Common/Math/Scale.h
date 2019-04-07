#pragma once
#include"Common.h"
#include"Vector.h"
#include"Matrix.h"

namespace Common
{
	class Transformation;
	class Scale
	{
		friend Transformation;
	private:
		JVector3   m_Scale;
		JMatrix4x4 m_ScaleMatrix;
	public:
		Scale() : m_Scale(1.0f, 1.0f, 1.0f) {}
		Scale(float x) : m_Scale(x, x, x) {}
		Scale(const Scale& copy) = default;
		Scale(Scale&& copy) = default;
		Scale& operator=(const Scale& rhs) = default;
		Scale& operator=(Scale&& rhs) = default;
		Scale(const JVector3& v) : m_Scale(v) {}
	public:
		void Set(const JVector3& v) {
			m_Scale = v;
			UpdateMatrix();
		}
		void Set(float x) {
			m_Scale = JVector3(x, x, x);
			UpdateMatrix();
		}

		void Offset(const JVector3& v) {
			m_Scale += v;
			UpdateMatrix();
		}
		void Offset(float x) {
			m_Scale += JVector3(x, x, x);
			UpdateMatrix();
		}
		void SetX(float x) {
			m_Scale.SetX(x);
			UpdateMatrix();
		}
		void SetY(float y) {
			m_Scale.SetY(y);
			UpdateMatrix();
		}
		void SetZ(float z) {
			m_Scale.SetZ(z);
			UpdateMatrix();
		}
		void OffsetX(float x) {
			m_Scale.SetX(m_Scale.X() + x);
			UpdateMatrix();
		}
		void OffsetY(float y) {
			m_Scale.SetY(m_Scale.Y() + y);
			UpdateMatrix();
		}
		void OffsetZ(float z) {
			m_Scale.SetZ(m_Scale.Z() + z);
			UpdateMatrix();
		}
		const JMatrix4x4& GetMatrix() const {
			return m_ScaleMatrix;
		}
		const JVector3& Get() const {
			return m_Scale;
		}
		float GetX() const {
			return m_Scale.X();
		}
		float GetY() const {
			return m_Scale.Y();
		}
		float GetZ() const {
			return m_Scale.Z();
		}
	public:
		bool operator==(const Scale& pos) const {
			return m_Scale == pos.m_Scale;
		}
		bool operator!=(const Scale& pos) const {
			return m_Scale != pos.m_Scale;
		}
	public:
		Scale operator+(const Scale& pos) const {
			return Scale(m_Scale + pos.m_Scale);
		}
		Scale operator+(float k) const {
			JVector3 result = m_Scale;
			result.AddScalar(k);
			return Scale(result);
		}
		Scale operator-(const Scale& pos) const {
			return Scale(m_Scale - pos.m_Scale);
		}
		Scale operator-(float k) const {
			JVector3 result = m_Scale;
			result.AddScalar(-k);
			return Scale(result);
		}
	public:
		Scale& operator+=(const Scale& pos) {
			m_Scale += pos.m_Scale;
			UpdateMatrix();
			return *this;
		}
		Scale& operator+=(float k) {
			m_Scale.AddScalar(k);
			UpdateMatrix();
			return *this;
		}
		Scale& operator-=(const Scale& pos) {
			m_Scale -= pos.m_Scale;
			UpdateMatrix();
			return *this;
		}
		Scale& operator-=(float k) {
			m_Scale.AddScalar(-k);
			UpdateMatrix();
			return *this;
		}
	public:
		Scale operator*(float k) const {
			return Scale(m_Scale * k);
		}
		Scale operator/(float k) const {
			return Scale(m_Scale / k);
		}
		Scale& operator*=(float k) {
			m_Scale *= k;
			UpdateMatrix();
			return *this;
		}
		Scale& operator/=(float k) {
			m_Scale /= k;
			UpdateMatrix();
			return *this;
		}
	private:
		void UpdateMatrix()
		{
			if (m_Scale.X() < 0.0f)
				m_Scale.SetX(0.0f);
			if (m_Scale.Y() < 0.0f)
				m_Scale.SetY(0.0f);
			if (m_Scale.Z() < 0.0f)
				m_Scale.SetZ(0.0f);

			m_ScaleMatrix.Scaling(m_Scale);
		}

	};
}
