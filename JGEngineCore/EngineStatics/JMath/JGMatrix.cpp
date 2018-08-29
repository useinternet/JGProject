#include"JGMatrix.h"
#include"JGScale2D.h"
#include"JGAngle2D.h"
#include"JGVector2D.h"
#include"JGVector3D.h"
#include"JGMath.h"
JGMatrix::JGMatrix()
{
	m_Matrix = {
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f
	};
}
JGMatrix::JGMatrix(const D3DXMATRIX& matrix)
{
	m_Matrix = matrix;
}
JGMatrix::JGMatrix(const JGMatrix& matrix)
{
	m_Matrix = matrix.m_Matrix;
}
JGMatrix::~JGMatrix()
{

}

void JGMatrix::Identity()
{
	D3DXMatrixIdentity(&m_Matrix);
}

void JGMatrix::Transpose()
{
	D3DXMatrixTranspose(&m_Matrix, &m_Matrix);
}

float JGMatrix::Inverse(JGMatrix * matrix)
{
	float result = 0.0f;
	D3DXMatrixInverse(&matrix->m_Matrix, &result, &m_Matrix);
	return result;
}

void JGMatrix::MakeScalingMatirx(const float x, const float y, const float z)
{
	D3DXMatrixScaling(&m_Matrix, x, y, z);
}

void JGMatrix::MakeScalingMatirx(JGScale2D& Scale)
{
	D3DXMatrixScaling(&m_Matrix, Scale.X(),Scale.Y(),1.0f);
}

void JGMatrix::MakeRotationMatirx(const float x, const float y, const float z)
{
	D3DXMatrixRotationYawPitchRoll(&m_Matrix, ToRadian(y), ToRadian(x), ToRadian(z));
}

void JGMatrix::MakeAngle2DMatrix(const float angle)
{
	D3DXMatrixRotationZ(&m_Matrix, ToRadian(angle));
}

void JGMatrix::MakeAngle2DMatrix(JGAngle2D& angle)
{
	D3DXMatrixRotationZ(&m_Matrix, ToRadian(angle.Get()));
}

void JGMatrix::MakeTranslationMatrix(const float x, const float y, const float z)
{
	D3DXMatrixTranslation(&m_Matrix, x, y, z);
}

void JGMatrix::MakeTranslation2DMatrix(const float ScreenWidth, const float ScreenHeight, const float x, const float y)
{
	JGVector2D FinalVector = JGMath::D2DToScreen(ScreenWidth, ScreenHeight, x, y);
	D3DXMatrixTranslation(&m_Matrix, FinalVector.X(), FinalVector.Y(), 0.0f);
}

void JGMatrix::MakeTranslation2DMatrix(const float ScreenWidth, const float ScreenHeight, JGVector2D& vector)
{
	JGVector2D FinalVector = JGMath::D2DToScreen(ScreenWidth, ScreenHeight, vector);
	D3DXMatrixTranslation(&m_Matrix, FinalVector.X(), FinalVector.Y(), 0.0f);
}

void JGMatrix::MakeViewMatrix(JGVector3D* location, JGVector3D* lookAt, JGVector3D* up)
{
	D3DXMatrixLookAtLH(&m_Matrix, location->GetAddress(), 
		lookAt->GetAddress(), up->GetAddress());
}

void JGMatrix::MakeProjectionMatrix(const float FOV, const float ScreenAspect, const float NearZ, const float FarZ)
{
	D3DXMatrixPerspectiveFovLH(&m_Matrix, ToRadian(FOV), ScreenAspect, NearZ, FarZ);
}

void JGMatrix::MakeOrthoMatrix(const float width, const float height, const float NearZ, const float FarZ)
{
	D3DXMatrixOrthoLH(&m_Matrix, width, height, NearZ, FarZ);
}

void JGMatrix::SetElement(const UINT slot, const float Value)
{
	switch (slot)
	{
	case 11:  m_Matrix._11 = Value;  break;
	case 12:  m_Matrix._12 = Value;  break;
	case 13:  m_Matrix._13 = Value;  break;
	case 14:  m_Matrix._14 = Value;  break;
	case 21:  m_Matrix._21 = Value;  break;
	case 22:  m_Matrix._22 = Value;  break;
	case 23:  m_Matrix._23 = Value;  break;
	case 24:  m_Matrix._24 = Value;  break;
	case 31:  m_Matrix._31 = Value;  break;
	case 32:  m_Matrix._32 = Value;  break;
	case 33:  m_Matrix._33 = Value;  break;
	case 34:  m_Matrix._34 = Value;  break;
	case 41:  m_Matrix._41 = Value;  break;
	case 42:  m_Matrix._42 = Value;  break;
	case 43:  m_Matrix._43 = Value;  break;
	case 44:  m_Matrix._44 = Value;  break;
	}
}

float JGMatrix::GetElement(const UINT slot)
{
	switch (slot)
	{
	case 11:  return m_Matrix._11;
	case 12:  return m_Matrix._12;
	case 13:  return m_Matrix._13;
	case 14:  return m_Matrix._14;
	case 21:  return m_Matrix._21;
	case 22:  return m_Matrix._22;
	case 23:  return m_Matrix._23;
	case 24:  return m_Matrix._24;
	case 31:  return m_Matrix._31;
	case 32:  return m_Matrix._32;
	case 33:  return m_Matrix._33;
	case 34:  return m_Matrix._34;
	case 41:  return m_Matrix._41;
	case 42:  return m_Matrix._42;
	case 43:  return m_Matrix._43;
	case 44:  return m_Matrix._44;
	default:
		return 0.0f;
	}
}

D3DXMATRIX& JGMatrix::Get()
{
	return m_Matrix;
}

D3DXMATRIX* JGMatrix::GetAddress()
{
	return &m_Matrix;
}

JGMatrix& JGMatrix::operator=(const JGMatrix& matrix)
{
	m_Matrix = matrix.m_Matrix;
	return *this;
}
JGMatrix JGMatrix::operator+(JGMatrix& matrix)
{
	return JGMatrix(m_Matrix + matrix.m_Matrix);
}
JGMatrix JGMatrix::operator-(JGMatrix& matrix)
{
	return JGMatrix(m_Matrix - matrix.m_Matrix);
}
JGMatrix JGMatrix::operator*(JGMatrix& matrix)
{
	return JGMatrix(m_Matrix * matrix.m_Matrix);
}

JGMatrix& JGMatrix::operator+=(JGMatrix& matrix)
{
	m_Matrix += matrix.m_Matrix;
	return *this;
}
JGMatrix& JGMatrix::operator-=(JGMatrix& matrix)
{
	m_Matrix -= matrix.m_Matrix;
	return *this;
}
JGMatrix& JGMatrix::operator*=(JGMatrix& matrix)
{
	m_Matrix *= matrix.m_Matrix;
	return *this;
}