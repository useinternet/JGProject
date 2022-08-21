#pragma once
#include "Math.h"
#include "String/String.h"
#include <DirectXMath.h>

class PVector3;
class PVector4;
class PQuaternion;

class PMatrix
{
	friend PVector3;
	using SimMat = DirectX::XMMATRIX;

private:
	DirectX::XMFLOAT4X4 m_Data;

public:
	PMatrix();
	PMatrix(float32 init_value);
	PMatrix(
		float32 _11, float32 _12, float32 _13, float32 _14,
		float32 _21, float32 _22, float32 _23, float32 _24,
		float32 _31, float32 _32, float32 _33, float32 _34,
		float32 _41, float32 _42, float32 _43, float32 _44) :
		m_Data(_11, _12, _13, _14,
			_21, _22, _23, _24,
			_31, _32, _33, _34,
			_41, _42, _43, _44) {}

	PMatrix(float32 w,
		const PVector3& v1,
		const PVector3& v2,
		const PVector3& v3,
		const PVector3& v4);

	PMatrix(
		const PVector3& v1, float32 w1,
		const PVector3& v2, float32 w2,
		const PVector3& v3, float32 w3,
		const PVector3& v4, float32 w4);

	PMatrix(const PMatrix& copy) = default;
	PMatrix(PMatrix&& rhs) = default;

	float32* GetFloatPtr() const 
	{
		return (float32*)m_Data.m;
	}

public: // ¿¬»êÀÚ
	PMatrix& operator=(const PMatrix& rhs) = default;
	PMatrix& operator=(PMatrix&& rhs)	   = default;

	PMatrix operator*(const PMatrix& m) const 
	{
		PMatrix result;
		DirectX::XMMATRIX sim_result = DirectX::XMMatrixMultiply(GetSIMD(), m.GetSIMD());
		result.SetSIMD(sim_result);
		return result;
	}

	PMatrix& operator*=(const PMatrix& m) 
	{
		DirectX::XMMATRIX sim_result = DirectX::XMMatrixMultiply(GetSIMD(), m.GetSIMD());
		SetSIMD(sim_result);
		return *this;
	}

	PMatrix operator*(float32 k)  const 
	{
		PMatrix m2(k);
		return (*this) * m2;
	}

	PMatrix& operator*=(float32 k) 
	{
		PMatrix m2(k);
		(*this) *= m2;
		return *this;
	}

	PVector3 TransformPoint(const PVector3& v)  const;
	PVector3 TransformVector(const PVector3& v) const;
	PVector4 Transform(const PVector4& v)       const;

	bool Decompose(PVector3* T, PQuaternion* R, PVector3* S) const;

public:
	static PMatrix Identity() 
	{
		PMatrix m;
		m.SetSIMD(DirectX::XMMatrixIdentity());
		return m;
	}

	static PMatrix Inverse(const PMatrix& m) 
	{
		PMatrix result;
		DirectX::XMVECTOR sim_det = DirectX::XMMatrixDeterminant(m.GetSIMD());
		DirectX::XMMATRIX sim_mat = DirectX::XMMatrixInverse(&sim_det, m.GetSIMD());
		result.SetSIMD(sim_mat);
		return result;
	}

	static PMatrix Transpose(const PMatrix& m) 
	{
		PMatrix result;
		result.SetSIMD(DirectX::XMMatrixTranspose(m.GetSIMD()));
		return result;
	}

	static PMatrix Translation(const PVector3& v);
	static PMatrix Rotation(const PVector3& v);
	static PMatrix RotationYaw(float32 yaw);
	static PMatrix RotationPitch(float32 pitch);
	static PMatrix RotationRoll(float32 roll);
	static PMatrix Rotation(const PQuaternion& q);
	static PMatrix RotationAxis(const PVector3& axis, float32 angle);
	static PMatrix Scaling(const PVector3& v);
	static PMatrix LookAtLH(const PVector3& pos, const PVector3& target, const PVector3& up);
	static PMatrix LookToLH(const PVector3& pos, const PVector3& targetDir, const PVector3& up);
	static PMatrix LookToLH(const PVector3& pos, const PVector3& up, const PVector3& right, const PVector3& look);
	static PVector3 ToEulerAngles(const PMatrix& m);

	static PMatrix PerspectiveFovLH(float32 fov, float32 aspectRatio, float32 nearZ, float32 farZ)
	{
		PMatrix result;
		result.SetSIMD(DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ));
		return result;
	}

	static PMatrix OrthographicLH(float32 width, float32 height, float32 nearZ, float32 farZ)
	{
		PMatrix result;
		result.SetSIMD(DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ));
		return result;
	}

	static PMatrix OrthographicOffCenterLH(float32 left, float32 right, float32 bottom, float32 top, float32 nearZ, float32 farZ)
	{
		PMatrix result;
		result.SetSIMD(DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearZ, farZ));
		return result;
	}

	static PMatrix AffineTransformation(const PVector3& T, const PQuaternion& Quat, const PVector3& S);

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

inline PMatrix operator*(float32 k, const PMatrix& m)
{
	return m * k;
}