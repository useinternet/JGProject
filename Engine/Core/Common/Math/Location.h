#pragma once
#include"Common.h"
#include"Vector.h"
#include"Matrix.h"


namespace Common
{
	class Transformation;
	class Location
	{
		friend Transformation;
	private:
		JVector3   m_Pos;
		JMatrix4x4 m_TransMatrix;
	public:
		Location() = default;
		Location(const Location& copy) = default;
		Location(Location&& copy) = default;
		Location& operator=(const Location& rhs) = default;
		Location& operator=(Location&& rhs) = default;
		Location(const JVector3& v) : m_Pos(v) {}
	public:
		void Set(const JVector3& v) {
			m_Pos = v;
			UpdateMatrix();
		}
		void Offset(const JVector3& v) {
			m_Pos += v;
			UpdateMatrix();
		}
		void SetX(float x) {
			m_Pos.SetX(x);
			UpdateMatrix();
		}
		void SetY(float y) {
			m_Pos.SetY(y);
			UpdateMatrix();
		}
		void SetZ(float z) {
			m_Pos.SetZ(z);
			UpdateMatrix();
		}
		void OffsetX(float x) {
			m_Pos.SetX(m_Pos.X() + x);
			UpdateMatrix();
		}
		void OffsetY(float y) {
			m_Pos.SetY(m_Pos.Y() + y);
			UpdateMatrix();
		}
		void OffsetZ(float z) {
			m_Pos.SetZ(m_Pos.Z() + z);
			UpdateMatrix();
		}
		const JMatrix4x4& GetMatrix() const {
			return m_TransMatrix;
		}
		const JVector3& Get() const {
			return m_Pos;
		}
		float GetX() const {
			return m_Pos.X();
		}
		float GetY() const {
			return m_Pos.Y();
		}
		float GetZ() const {
			return m_Pos.Z();
		}
		float Distance(const Location& l) const {
			return (l.m_Pos - m_Pos).Length();
		}
		float DistanceSquard(const Location& l) const {
			return (l.m_Pos - m_Pos).LengthSquard();
		}
	public:
		Location operator-() const {
			return Location(-m_Pos);
		}
		bool operator==(const Location& pos) const {
			return m_Pos == pos.m_Pos;
		}
		bool operator!=(const Location& pos) const {
			return m_Pos != pos.m_Pos;
		}
	public:
		Location operator+(const Location& pos) const {
			return Location(m_Pos + pos.m_Pos);
		}
		Location operator+(float k) const {
			JVector3 result = m_Pos;
			result.AddScalar(k);
			return Location(result);
		}
		Location operator-(const Location& pos) const {
			return Location(m_Pos - pos.m_Pos);
		}
		Location operator-(float k) const{
			JVector3 result = m_Pos;
			result.AddScalar(-k);
			return Location(result);
		}
	public:
		Location& operator+=(const Location& pos) {
			m_Pos += pos.m_Pos;
			UpdateMatrix();
			return *this;
		}
		Location& operator+=(float k) {
			m_Pos.AddScalar(k);
			UpdateMatrix();
			return *this;
		}
		Location& operator-=(const Location& pos) {
			m_Pos -= pos.m_Pos;
			UpdateMatrix();
			return *this;
		}
		Location& operator-=(float k) {
			m_Pos.AddScalar(-k);
			UpdateMatrix();
			return *this;
		}
	public:
		Location operator*(float k) const {
			return Location(m_Pos * k);
		}
		Location operator/(float k) const {
			return Location(m_Pos / k);
		}
		Location& operator*=(float k) {
			m_Pos *= k;
			UpdateMatrix();
			return *this;
		}
		Location& operator/=(float k) {
			m_Pos /= k;
			UpdateMatrix();
			return *this;
		}
	private:
		void UpdateMatrix()
		{
			m_TransMatrix.Translation(m_Pos);
		}

	};
}
