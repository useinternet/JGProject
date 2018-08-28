#include"JGScale2D.h"
#include"JGMath.h"
JGScale2D::JGScale2D()
{

}
JGScale2D::JGScale2D(const float flatNum)
{
	m_Scale2D.x = flatNum;
	m_Scale2D.y = flatNum;
}
JGScale2D::JGScale2D(const float x, const float y)
{
	m_Scale2D.x = x;
	m_Scale2D.y = y;
}
JGScale2D::JGScale2D(const JGScale2D& copy)
{
	m_Scale2D = copy.m_Scale2D;
}
JGScale2D::~JGScale2D()
{

}
float JGScale2D::X()
{
	return m_Scale2D.x;
}
float JGScale2D::Y()
{
	return m_Scale2D.y;
}
void JGScale2D::Set(const float flatName)
{
	m_Scale2D.x = flatName;
	m_Scale2D.y = flatName;
}
void JGScale2D::Set(const float x, const float y)
{
	m_Scale2D.x = x;
	m_Scale2D.y = y;
}
/*
Exp : x값 y값 셋팅 */
void JGScale2D::Set(const JGScale2D& scale)
{
	m_Scale2D = scale.m_Scale2D;
}
bool JGScale2D::IsZeroScale()
{
	if (JGMath::IsZero(m_Scale2D.x) && JGMath::IsZero(m_Scale2D.y))
	{
		return true;
	}
	return false;
}

JGScale2D& JGScale2D::operator=(JGScale2D& scale)
{
	m_Scale2D = scale.m_Scale2D;
	return *this;
}

JGScale2D JGScale2D::operator+(JGScale2D& scale)
{
	return JGScale2D(m_Scale2D.x + scale.m_Scale2D.x, m_Scale2D.y + scale.m_Scale2D.y);
}

JGScale2D JGScale2D::operator-(JGScale2D& scale)
{
	return JGScale2D(m_Scale2D.x - scale.m_Scale2D.x, m_Scale2D.y - scale.m_Scale2D.y);
}

bool JGScale2D::operator==(JGScale2D& scale)
{
	if (IsZeroScale() && scale.IsZeroScale())
	{
		return true;
	}
	if (JGMath::Equal(m_Scale2D.x, scale.m_Scale2D.x) &&
		JGMath::Equal(m_Scale2D.y, scale.m_Scale2D.y))
	{
		return true;
	}
	return false;
}

bool JGScale2D::operator!=(JGScale2D & scale)
{
	bool result = this->operator==(scale);
	return !result;
}

JGScale2D operator*(JGScale2D& scale, const float scalar)
{
	return JGScale2D(scale.m_Scale2D.x * scalar, scale.m_Scale2D.y * scalar);
}

JGScale2D operator*(const float scalar, JGScale2D& scale)
{
	return JGScale2D(scale.m_Scale2D.x * scalar, scale.m_Scale2D.y * scalar);
}
