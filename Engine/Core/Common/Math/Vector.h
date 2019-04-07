#pragma once


#include"Common.h"

namespace Common
{
	typedef DirectX::XMVECTOR  SimVector;
	typedef DirectX::FXMVECTOR FSimVector;
	typedef DirectX::XMFLOAT3  DxVector3f;
	typedef DirectX::XMFLOAT4  DxVector4f;
	class JVector3
	{
		friend JVector3 operator*(float k, const JVector3& v);
		friend JVector3 operator/(float k, const JVector3& v);
	private:
		SimVector m_SimVec;
	public: // 생성자 
		JVector3() {
			m_SimVec = DirectX::XMVectorZero();
		}
		JVector3(const JVector3& copy) {
			m_SimVec = copy.m_SimVec;
		}
		JVector3(JVector3&& rhs) {
			m_SimVec = rhs.m_SimVec;
		}
		JVector3(FSimVector vec) {
			m_SimVec = vec;
		}
		JVector3(float x, float y, float z) {
			Set(x, y, z);
		}
		JVector3(const DxVector3f& v) {
			Set(v);
		}
	public:
		JVector3& operator=(const JVector3& v) {
			m_SimVec = v.m_SimVec;
			return *this;
		}
		JVector3& operator=(JVector3&& v) {
			m_SimVec = v.m_SimVec;
			return *this;
		}
		JVector3 operator-() const {
			return JVector3(DirectX::XMVectorNegate(m_SimVec));
		}

	public: //  +, - 
		JVector3 operator+(const JVector3& v) const {
			return JVector3(DirectX::XMVectorAdd(m_SimVec, v.m_SimVec));
		}
		JVector3 operator-(const JVector3& v) const {
			return JVector3(DirectX::XMVectorSubtract(m_SimVec, v.m_SimVec));
		}
	public: // +, -
		JVector3& operator+=(const JVector3& v) {
			m_SimVec = DirectX::XMVectorAdd(m_SimVec, v.m_SimVec);
			return *this;
		}
		JVector3& operator-=(const JVector3& v) {
			m_SimVec = DirectX::XMVectorSubtract(m_SimVec, v.m_SimVec);
			return *this;
		}
	public: // 스칼라 *,/
		JVector3 operator*(float k) const {
			return JVector3(DirectX::XMVectorMultiply(m_SimVec, DirectX::XMVectorSet(k, k, k, 1.0f)));
		}
		JVector3 operator/(float k) const {
			return JVector3(DirectX::XMVectorDivide(m_SimVec, DirectX::XMVectorSet(k, k, k, 1.0f)));
		}
	public:
		JVector3& operator*=(float k)  {
			m_SimVec = DirectX::XMVectorMultiply(m_SimVec, DirectX::XMVectorSet(k, k, k, 1.0f));
			return *this;
		}
		JVector3& operator/=(float k)  {
			m_SimVec = DirectX::XMVectorDivide(m_SimVec, DirectX::XMVectorSet(k, k, k, 1.0f));
			return *this;
		}

	public: // 비교 연산자
		bool operator==(const JVector3& v) const {
			return DirectX::XMVectorGetX(DirectX::XMVectorEqual(m_SimVec, v.m_SimVec));
		}
		bool operator!=(const JVector3& v) const {
			return !DirectX::XMVectorGetX(DirectX::XMVectorEqual(m_SimVec, v.m_SimVec));
		}
	public: // 편의 함수
		void AddScalar(float x) {
			m_SimVec = DirectX::XMVectorAdd(m_SimVec, DirectX::XMVectorSet(x, x, x, 0.0f));
		}
		float Length() const {
			return DirectX::XMVectorGetX(DirectX::XMVector3Length(m_SimVec));
		}
		float LengthSquard() const {
			return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(m_SimVec));
		}
		void Normalize() {
			m_SimVec = DirectX::XMVector3Normalize(m_SimVec);
		}
	public: // 접근 함수
		void Set(float x , float y, float z) {
			m_SimVec = DirectX::XMVectorSet(x, y, z, 1.0f);
		}
		void Set(const JVector3& v) {
			m_SimVec = v.m_SimVec;
		}
		void Set(const DxVector3f& v) {
			m_SimVec = DirectX::XMLoadFloat3(&v);
		}
		DxVector3f Get() const {
			DxVector3f result;
			DirectX::XMStoreFloat3(&result, m_SimVec);
			return result;
		}
		SimVector GetSIMD() const {
			return m_SimVec;
		}
		void SetX(float x) {
			m_SimVec = DirectX::XMVectorSetX(m_SimVec, x);
		}
		void SetY(float y) {
			m_SimVec = DirectX::XMVectorSetY(m_SimVec, y);
		}
		void SetZ(float z) {
			m_SimVec = DirectX::XMVectorSetZ(m_SimVec, z);
		}
		float X() const {
			return DirectX::XMVectorGetX(m_SimVec);
		}
		float Y() const {
			return DirectX::XMVectorGetY(m_SimVec);
		}
		float Z() const {
			return DirectX::XMVectorGetZ(m_SimVec);
		}

	};



	JVector3 operator*(float k, const JVector3& v); 
	JVector3 operator/(float k, const JVector3& v); 



	float Dot(const JVector3& v1, const JVector3& v2); 
	JVector3 Normalize(const JVector3& v);
	JVector3 Cross(const JVector3& v1, const JVector3& v2); 
}