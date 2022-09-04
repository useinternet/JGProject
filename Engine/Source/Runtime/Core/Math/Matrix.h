#pragma once
#include "Math.h"
#include "String/String.h"
#include <DirectXMath.h>

class HVector3;
class HVector4;
class HQuaternion;

class HMatrix
{
	friend HVector3;
	using SimMat = DirectX::XMMATRIX;

private:
	DirectX::XMFLOAT4X4 m_Data;

public:
	HMatrix();
	HMatrix(float32 init_value);
	HMatrix(
		float32 _11, float32 _12, float32 _13, float32 _14,
		float32 _21, float32 _22, float32 _23, float32 _24,
		float32 _31, float32 _32, float32 _33, float32 _34,
		float32 _41, float32 _42, float32 _43, float32 _44) :
		m_Data(_11, _12, _13, _14,
			_21, _22, _23, _24,
			_31, _32, _33, _34,
			_41, _42, _43, _44) {}

	HMatrix(float32 w,
		const HVector3& v1,
		const HVector3& v2,
		const HVector3& v3,
		const HVector3& v4);

	HMatrix(
		const HVector3& v1, float32 w1,
		const HVector3& v2, float32 w2,
		const HVector3& v3, float32 w3,
		const HVector3& v4, float32 w4);

	HMatrix(const HMatrix& copy) = default;
	HMatrix(HMatrix&& rhs) = default;

	float32* GetFloatPtr() const 
	{
		return (float32*)m_Data.m;
	}

public: // ¿¬»êÀÚ
	HMatrix& operator=(const HMatrix& rhs) = default;
	HMatrix& operator=(HMatrix&& rhs)	   = default;

	HMatrix operator*(const HMatrix& m) const 
	{
		HMatrix result;
		DirectX::XMMATRIX sim_result = DirectX::XMMatrixMultiply(GetSIMD(), m.GetSIMD());
		result.SetSIMD(sim_result);
		return result;
	}

	HMatrix& operator*=(const HMatrix& m) 
	{
		DirectX::XMMATRIX sim_result = DirectX::XMMatrixMultiply(GetSIMD(), m.GetSIMD());
		SetSIMD(sim_result);
		return *this;
	}

	HMatrix operator*(float32 k)  const 
	{
		HMatrix m2(k);
		return (*this) * m2;
	}

	HMatrix& operator*=(float32 k) 
	{
		HMatrix m2(k);
		(*this) *= m2;
		return *this;
	}

	HVector3 TransformPoint(const HVector3& v)  const;
	HVector3 TransformVector(const HVector3& v) const;
	HVector4 Transform(const HVector4& v)       const;

	bool Decompose(HVector3* T, HQuaternion* R, HVector3* S) const;

public:
	static HMatrix Identity() 
	{
		HMatrix m;
		m.SetSIMD(DirectX::XMMatrixIdentity());
		return m;
	}

	static HMatrix Inverse(const HMatrix& m) 
	{
		HMatrix result;
		DirectX::XMVECTOR sim_det = DirectX::XMMatrixDeterminant(m.GetSIMD());
		DirectX::XMMATRIX sim_mat = DirectX::XMMatrixInverse(&sim_det, m.GetSIMD());
		result.SetSIMD(sim_mat);
		return result;
	}

	static HMatrix Transpose(const HMatrix& m) 
	{
		HMatrix result;
		result.SetSIMD(DirectX::XMMatrixTranspose(m.GetSIMD()));
		return result;
	}

	static HMatrix Translation(const HVector3& v);
	static HMatrix Rotation(const HVector3& v);
	static HMatrix RotationYaw(float32 yaw);
	static HMatrix RotationPitch(float32 pitch);
	static HMatrix RotationRoll(float32 roll);
	static HMatrix Rotation(const HQuaternion& q);
	static HMatrix RotationAxis(const HVector3& axis, float32 angle);
	static HMatrix Scaling(const HVector3& v);
	static HMatrix LookAtLH(const HVector3& pos, const HVector3& target, const HVector3& up);
	static HMatrix LookToLH(const HVector3& pos, const HVector3& targetDir, const HVector3& up);
	static HMatrix LookToLH(const HVector3& pos, const HVector3& up, const HVector3& right, const HVector3& look);
	static HVector3 ToEulerAngles(const HMatrix& m);

	static HMatrix PerspectiveFovLH(float32 fov, float32 aspectRatio, float32 nearZ, float32 farZ)
	{
		HMatrix result;
		result.SetSIMD(DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ));
		return result;
	}

	static HMatrix OrthographicLH(float32 width, float32 height, float32 nearZ, float32 farZ)
	{
		HMatrix result;
		result.SetSIMD(DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ));
		return result;
	}

	static HMatrix OrthographicOffCenterLH(float32 left, float32 right, float32 bottom, float32 top, float32 nearZ, float32 farZ)
	{
		HMatrix result;
		result.SetSIMD(DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearZ, farZ));
		return result;
	}

	static HMatrix AffineTransformation(const HVector3& T, const HQuaternion& Quat, const HVector3& S);

public:
	float32& Get(int32 col, int32 row) 
	{
		return m_Data.m[row][col];
	}

	float32 Get_C(int32 col, int32 row) const 
	{
		return m_Data.m[row][col];
	}

	PString ToString() const 
	{
		return PString::Format(R"([ %3.f, %3.f, %3.f, %3.f ]
[ %3.f, %3.f, %3.f, %3.f ],
[ %3.f, %3.f, %3.f, %3.f ],
[ %3.f, %3.f, %3.f, %3.f ])",
m_Data._11, m_Data._12, m_Data._13, m_Data._14,
m_Data._21, m_Data._22, m_Data._23, m_Data._24,
m_Data._31, m_Data._32, m_Data._33, m_Data._34,
m_Data._41, m_Data._42, m_Data._43, m_Data._44);

	}

private:
	SimMat GetSIMD() const
	{
		return DirectX::XMLoadFloat4x4(&m_Data);
	}

	void SetSIMD(SimMat sim_mat)
	{
		DirectX::XMStoreFloat4x4(&m_Data, sim_mat);
	}
};

inline HMatrix operator*(float32 k, const HMatrix& m)
{
	return m * k;
}