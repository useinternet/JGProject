#pragma once
#include"Common.h"


namespace Common
{
	typedef DirectX::XMFLOAT4X4 DxMatrix4x4f;
	typedef DirectX::XMMATRIX   SimMatrix;
	typedef DirectX::FXMMATRIX  FSimMatrix;
	class JVector3;
	class JMatrix4x4
	{
	private:
		SimMatrix m_SimMat;
	public:
		JMatrix4x4() {
			m_SimMat = DirectX::XMMatrixIdentity();
		}
		JMatrix4x4(float m00, float m01, float m02, float m03,
		          float m10, float m11, float m12, float m13,
			      float m20, float m21, float m22, float m23,
			      float m30, float m31, float m32, float m33) {
			m_SimMat = DirectX::XMMatrixSet(
				m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33);
		}
		JMatrix4x4(const JMatrix4x4& copy) {
			m_SimMat = copy.m_SimMat;
		}
		JMatrix4x4(JMatrix4x4&& rhs) {
			m_SimMat = rhs.m_SimMat;
		}
		JMatrix4x4(FSimMatrix simMat) {
			m_SimMat = simMat;
		}
		JMatrix4x4(const DxMatrix4x4f& m) {
			m_SimMat = DirectX::XMLoadFloat4x4(&m);
		}
	public:
		JMatrix4x4& operator=(const JMatrix4x4& m) {
			m_SimMat = m.m_SimMat;
			return *this;
		}
		JMatrix4x4& operator=(JMatrix4x4&& m) {
			m_SimMat = m.m_SimMat;
			return *this;
		}
	public:
		JMatrix4x4 operator*(const JMatrix4x4& m) const {
			return JMatrix4x4(DirectX::XMMatrixMultiply(m_SimMat, m.m_SimMat));
		}
		JMatrix4x4& operator*=(const JMatrix4x4& m) {
			m_SimMat = DirectX::XMMatrixMultiply(m_SimMat, m.m_SimMat);
			return *this;
		}
	public:
		float Determinant() const {
			return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(m_SimMat));
		}
		void Identity() {
			m_SimMat = DirectX::XMMatrixIdentity();
		}
		void Translation(float x, float y, float z) {
			m_SimMat = DirectX::XMMatrixTranslation(x, y, z);
		}
		void Rotation(float pitch, float yaw, float roll) {
			m_SimMat = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		}
		void RotationX(float x) {
			m_SimMat = DirectX::XMMatrixRotationX(x);
		}
		void RotationY(float y) {
			m_SimMat = DirectX::XMMatrixRotationY(y);
		}
		void RotationZ(float z) {
			m_SimMat = DirectX::XMMatrixRotationZ(z);
		}

		void Scaling(float x, float y, float z) {
			m_SimMat = DirectX::XMMatrixScaling(x, y, z);
		}
		void PerspectiveFovLH(float fov, float aspectRatio, float nearZ, float farZ) {
			m_SimMat = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
		}
		void OrthographicLH(float width, float height, float nearZ, float farZ){
			m_SimMat = DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ);
		}
		void Transpose() {
			m_SimMat = DirectX::XMMatrixTranspose(m_SimMat);
		}
		
		void Translation(const JVector3& v);
		void Rotation(const JVector3& v);
		void RotationAxis(const JVector3& axis, float angle);
		void Scaling(const JVector3& v);
		void LookAtLH(const JVector3& pos, const JVector3& target, const JVector3& up);
		void Inverse();
		JVector3 TransformPoint(const JVector3& v) const;
		JVector3 TransformVector(const JVector3& v) const;
	public:
		DxMatrix4x4f Get() const {
			DxMatrix4x4f result;
			DirectX::XMStoreFloat4x4(&result, m_SimMat);
			return result;
		}
		void  Set(int col, int row, float value) {
			assert(col < 4 && "JMatrix4x4's col value is over 3");
			switch (row) {
			case 0:
				m_SimMat.r[col] = DirectX::XMVectorSetX(m_SimMat.r[col], value);
				break;
			case 1:
				m_SimMat.r[col] = DirectX::XMVectorSetY(m_SimMat.r[col], value);
				break;
			case 2:
				m_SimMat.r[col] = DirectX::XMVectorSetZ(m_SimMat.r[col], value);
				break;
			case 3:
				m_SimMat.r[col] = DirectX::XMVectorSetW(m_SimMat.r[col], value);
				break;
			default:
				assert(false && "JMatrix4x4's row value is over 3");
			}
		}
		float Get(int col, int row) const {
			assert(col < 4 && "JMatrix4x4's col value is over 3");
			switch (row) {
			case 0:
				return DirectX::XMVectorGetX(m_SimMat.r[col]);
				break;
			case 1:
				return DirectX::XMVectorGetY(m_SimMat.r[col]);
				break;
			case 2:
				return DirectX::XMVectorGetZ(m_SimMat.r[col]);
				break;
			case 3:
				return DirectX::XMVectorGetW(m_SimMat.r[col]);
				break;
			default:
				assert(false && "JMatrix4x4's row value is over 3");
			}
		}
	};

	JMatrix4x4 Identity();
	JMatrix4x4 Translation(float x, float y, float z);
	JMatrix4x4 Rotation(float pitch, float yaw, float roll);
	JMatrix4x4 RotationX(float x);
	JMatrix4x4 RotationY(float y);
	JMatrix4x4 RotationZ(float z);
	JMatrix4x4 Scaling(float x, float y, float z);
	JMatrix4x4 PerspectiveFovLH(float fov, float aspectRatio, float nearZ, float farZ);
	JMatrix4x4 OrthographicLH(float width, float height, float nearZ, float farZ);
	JMatrix4x4 Transpose(const JMatrix4x4& matrix);
	JMatrix4x4 Translation(const JVector3& v);
	JMatrix4x4 Rotation(const JVector3& v);
	JMatrix4x4 RotationAxis(const JVector3& axis, float angle);
	JMatrix4x4 Scaling(const JVector3& v);
	JMatrix4x4 LookAtLH(const JVector3& pos, const JVector3& target, const JVector3& up);
	JMatrix4x4 Inverse(const JMatrix4x4& matrix);
	JVector3 TransformPoint(const JMatrix4x4& m, const JVector3& v);
	JVector3 TransformVector(const JMatrix4x4& m ,const JVector3& v);


}