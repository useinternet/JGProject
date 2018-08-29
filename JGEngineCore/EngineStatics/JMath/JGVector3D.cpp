#include"JGVector3D.h"
#include"JGMath.h"
#include"JGMatrix.h"

JGVector3D::JGVector3D()
{
}

JGVector3D::JGVector3D(const float x, const float y, const float z)
{
	m_Vector3D.x = x;
	m_Vector3D.y = y;
	m_Vector3D.z = z;
}

JGVector3D::JGVector3D(const JGVector3D & copy)
{
	m_Vector3D = copy.m_Vector3D;
}

JGVector3D::~JGVector3D()
{
}


bool JGVector3D::IsZeroVector()
{
	if (JGMath::IsZero(m_Vector3D.x) && JGMath::IsZero(m_Vector3D.y) &&
		JGMath::IsZero(m_Vector3D.z))
	{
		return true;
	}
	return false;
}
JGVector3D  JGVector3D::Dot(JGVector3D& Vector)
{
	return JGVector3D(Vector.m_Vector3D.x * m_Vector3D.x, Vector.m_Vector3D.y * m_Vector3D.y,
		m_Vector3D.z * Vector.m_Vector3D.z);
}
void JGVector3D::Normalize()
{
	float lengthSqrd = LengthSquard();
	if (JGMath::IsZero(lengthSqrd))
	{
		m_Vector3D = { 0.0f,0.0f,0.0f };
		return;
	}

	float InverselengthSqrd = 1 / Length();
	m_Vector3D.x *= InverselengthSqrd;
	m_Vector3D.y *= InverselengthSqrd;
	m_Vector3D.z *= InverselengthSqrd;
}
float JGVector3D::Length()
{
	return JGMath::Sqrt(LengthSquard());
}
float JGVector3D::LengthSquard()
{
	return (m_Vector3D.x * m_Vector3D.x) + (m_Vector3D.y * m_Vector3D.y) + (m_Vector3D.z * m_Vector3D.z);
}

float JGVector3D::X()
{
	return m_Vector3D.x;
}

float JGVector3D::Y()
{
	return m_Vector3D.y;
}

float JGVector3D::Z()
{
	return m_Vector3D.z;
}

void JGVector3D::Set(const float x, const float y,const float z)
{
	m_Vector3D.x = x;
	m_Vector3D.y = y;
	m_Vector3D.z = z;
}
void JGVector3D::Set(const JGVector3D & vec)
{
	m_Vector3D.x = vec.m_Vector3D.x;
	m_Vector3D.y = vec.m_Vector3D.y;
	m_Vector3D.z = vec.m_Vector3D.z;
}
D3DXVECTOR3& JGVector3D::Get()
{
	return m_Vector3D;
}
D3DXVECTOR3* JGVector3D::GetAddress()
{
	return &m_Vector3D;
}
JGVector3D& JGVector3D::operator=(JGVector3D& vec)
{
	m_Vector3D = vec.m_Vector3D;
	return *this;
}
JGVector3D JGVector3D::operator+(JGVector3D& vec)
{
	return JGVector3D(m_Vector3D.x + vec.m_Vector3D.x,
		m_Vector3D.y + vec.m_Vector3D.y,
		m_Vector3D.z + vec.m_Vector3D.z);

}
JGVector3D JGVector3D::operator-(JGVector3D& vec)
{
	return JGVector3D(m_Vector3D.x - vec.m_Vector3D.x,
		m_Vector3D.y - vec.m_Vector3D.y,
		m_Vector3D.z - vec.m_Vector3D.z);
}

bool JGVector3D::operator==(JGVector3D & vec)
{
	if (IsZeroVector() && vec.IsZeroVector())
	{
		return true;
	}
	if (JGMath::Equal(m_Vector3D.x, vec.m_Vector3D.x) &&
		JGMath::Equal(m_Vector3D.y, vec.m_Vector3D.y) &&
		JGMath::Equal(m_Vector3D.z, vec.m_Vector3D.z))
	{
		return true;
	}
	return false;
}

bool JGVector3D::operator!=(JGVector3D & vec)
{
	bool result = this->operator==(vec);
	return !result;
}
void JGVector3D::TransformCoord(JGMatrix& matrix)
{
	D3DXVec3TransformCoord(&m_Vector3D, &m_Vector3D, matrix.GetAddress());
}
void JGVector3D::TransformNormal(JGMatrix& matrix)
{
	D3DXVec3TransformNormal(&m_Vector3D, &m_Vector3D, matrix.GetAddress());
}
JGVector3D operator*(JGVector3D & vec, const float scalar)
{
	return JGVector3D(vec.m_Vector3D.x * scalar, vec.m_Vector3D.y * scalar,
		vec.m_Vector3D.z * scalar);
}

JGVector3D operator*(const float scalar, JGVector3D & vec)
{
	return JGVector3D(vec.m_Vector3D.x * scalar, vec.m_Vector3D.y * scalar,
		vec.m_Vector3D.z * scalar);
}