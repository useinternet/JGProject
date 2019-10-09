#include "pch.h"
#include "JMatrix.h"
#include "JVector.h"


JMatrix::JMatrix() :
	m_Data(
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f) {}
JMatrix::JMatrix(float init_value) : 
	m_Data(
		init_value, init_value, init_value, init_value,
		init_value, init_value, init_value, init_value,
		init_value, init_value, init_value, init_value,
		init_value, init_value, init_value, init_value) {}

JMatrix::JMatrix(
	float w,
	const JVector& v1,
	const JVector& v2,
	const JVector& v3,
	const JVector& v4) :
	m_Data(
		v1.x, v1.y, v1.z, w,
		v2.x, v2.y, v2.z, w,
		v3.x, v3.y, v3.z, w,
		v4.x, v4.y, v4.z, w) {};


JMatrix::JMatrix(
	const JVector& v1, float w1,
	const JVector& v2, float w2,
	const JVector& v3, float w3,
	const JVector& v4, float w4) :
	m_Data(
		v1.x, v1.y, v1.z, w1,
		v2.x, v2.y, v2.z, w2,
		v3.x, v3.y, v3.z, w3,
		v4.x, v4.y, v4.z, w4) { }


JMatrix JMatrix::Translation(const JVector& v)
{
	JMatrix m;
	m.SetSIMD(DirectX::XMMatrixTranslation(v.x, v.y, v.z));
	return m;
}
JMatrix JMatrix::Rotation(const JVector& v)
{
	JMatrix m;
	m.SetSIMD(DirectX::XMMatrixRotationRollPitchYaw(v.x, v.y, v.z));
	return m;
}
JMatrix JMatrix::LookAtLH(const JVector& pos, const JVector& target, const JVector& up)
{
	JMatrix m;
	m.SetSIMD(DirectX::XMMatrixLookAtLH(
		JVector::GetSIMD(pos),
		JVector::GetSIMD(target),
		JVector::GetSIMD(up)));
	return m;
}