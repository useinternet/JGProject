#pragma once
#include "CommonCore.h"
#include <DirectXMath.h>

class JVector;
class JMatrix
{
	using SimMat = DirectX::XMMATRIX;
private:
	DirectX::XMFLOAT4X4 m_Data;
public:
	JMatrix();
	JMatrix(float init_value);
	JMatrix(
		float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44) :
		m_Data(_11, _12, _13, _14,
			_21, _22, _23, _24,
			_31, _32, _33, _34,
			_41, _42, _43, _44) {}
	JMatrix(float w,
		const JVector& v1,
		const JVector& v2,
		const JVector& v3,
		const JVector& v4);
	JMatrix(
		const JVector& v1, float w1,
		const JVector& v2, float w2,
		const JVector& v3, float w3,
		const JVector& v4, float w4);

	JMatrix(const JMatrix& copy) = default;
	JMatrix(JMatrix&& rhs) = default;
public: // ¿¬»êÀÚ
	inline JMatrix& operator=(const JMatrix& rhs) = default;
	inline JMatrix& operator=(JMatrix&& rhs) = default;

	inline JMatrix operator*(const JMatrix& m) const {
		JMatrix result;
		auto sim_result = DirectX::XMMatrixMultiply(GetSIMD(), m.GetSIMD());
		result.SetSIMD(sim_result);
		return result;
	}
	inline JMatrix& operator*=(const JMatrix& m) {
		auto sim_result = DirectX::XMMatrixMultiply(GetSIMD(), m.GetSIMD());
		SetSIMD(sim_result);
		return *this;
	}


	inline JMatrix operator*(float k)  const {
		JMatrix m2(k);
		return (*this) * m2;
	}
	inline JMatrix& operator*=(float k) {
		JMatrix m2(k);
		(*this) *= m2;
		return *this;
	}
public:
	inline static JMatrix Identity() {
		JMatrix m;
		m.SetSIMD(DirectX::XMMatrixIdentity());
		return m;
	}
	inline static JMatrix Inverse(const JMatrix& m) {
		JMatrix result;
		auto sim_mat = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(m.GetSIMD()), m.GetSIMD());
		result.SetSIMD(sim_mat);
		return result;
	}
	inline static JMatrix Transpose(const JMatrix& m) {
		JMatrix result;
		result.SetSIMD(DirectX::XMMatrixTranspose(m.GetSIMD()));
		return result;
	}
	static JMatrix Translation(const JVector& v);
	static JMatrix Rotation(const JVector& v);
	static JMatrix LookAtLH(const JVector& pos, const JVector& target, const JVector& up);
	inline static JMatrix PerspectiveFovLH(float fov, float aspectRatio, float nearZ, float farZ)
	{
		JMatrix result;
		result.SetSIMD(DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ));
		return result;
	}
	inline static JMatrix OrthographicLH(float width, float height, float nearZ, float farZ)
	{
		JMatrix result;
		result.SetSIMD(DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ));
		return result;
	}
public:
	float& Get(int col, int row) {
		return m_Data.m[col][row];
	}
	float Get_C(int col, int row) const {
		return m_Data.m[col][row];
	}
	std::string ToString() const{
		return "[ " + std::to_string(m_Data._11) + ", " + std::to_string(m_Data._12) + ", " + std::to_string(m_Data._13) + ", " + std::to_string(m_Data._14) + " ]\n" +
			   "[ " + std::to_string(m_Data._21) + ", " + std::to_string(m_Data._22) + ", " + std::to_string(m_Data._23) + ", " + std::to_string(m_Data._24) + " ]\n" +
			   "[ " + std::to_string(m_Data._31) + ", " + std::to_string(m_Data._32) + ", " + std::to_string(m_Data._33) + ", " + std::to_string(m_Data._34) + " ]\n" +
			   "[ " + std::to_string(m_Data._41) + ", " + std::to_string(m_Data._42) + ", " + std::to_string(m_Data._43) + ", " + std::to_string(m_Data._44) + " ]\n";
	}
private:
	inline SimMat GetSIMD() const {
		return DirectX::XMLoadFloat4x4(&m_Data);
	}
	inline void SetSIMD(SimMat sim_mat) {
		DirectX::XMStoreFloat4x4(&m_Data, sim_mat);
	}
};


inline JMatrix operator*(float k, const JMatrix& m) {
	return m * k;
}