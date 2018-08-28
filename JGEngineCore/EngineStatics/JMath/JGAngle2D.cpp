#include"JGAngle2D.h"
#include"JGMath.h"

JGAngle2D::JGAngle2D()
{

}
JGAngle2D::JGAngle2D(float angle)
{
	m_Angle = angle;
}
JGAngle2D::JGAngle2D(const JGAngle2D& Angle)
{
	m_Angle = Angle.m_Angle;
}
JGAngle2D::~JGAngle2D()
{

}

float JGAngle2D::Get()
{
	return m_Angle;
}
void  JGAngle2D::Set(const float Angle)
{
	m_Angle = Angle;
}
void  JGAngle2D::Set(const JGAngle2D& Angle)
{
	m_Angle = Angle.m_Angle;
}
void  JGAngle2D::StartState()
{
	if (m_Angle > 360.0f)
	{
		m_Angle = m_Angle - 360.0f;
	}
	if (m_Angle < -360.0f)
	{
		m_Angle = m_Angle + 360.0f;
	}
}

JGAngle2D& JGAngle2D::operator++()
{
	m_Angle += 1.0f;
	return *this;
}
JGAngle2D JGAngle2D::operator++(const int num)
{
	m_Angle += 1.0f;
	return JGAngle2D(m_Angle);
}

JGAngle2D& JGAngle2D::operator+=(JGAngle2D& angle)
{
	m_Angle += angle.m_Angle;
	return *this;
}
JGAngle2D& JGAngle2D::operator-=(JGAngle2D& angle)
{
	m_Angle -= angle.m_Angle;
	return *this;
}
JGAngle2D& JGAngle2D::operator+=(const float num)
{
	m_Angle += num;
	return *this;
}
JGAngle2D& JGAngle2D::operator-=(const float num)
{
	m_Angle -= num;
	return *this;
}
JGAngle2D& JGAngle2D::operator*=(const float num)
{
	m_Angle *= num;
	return *this;
}

JGAngle2D& JGAngle2D::operator=(JGAngle2D& angle)
{
	m_Angle = angle.m_Angle;
	return *this;
}
JGAngle2D JGAngle2D::operator+(JGAngle2D& angle)
{
	return JGAngle2D(m_Angle + angle.m_Angle);
}
JGAngle2D JGAngle2D::operator-(JGAngle2D& angle)
{
	return JGAngle2D(m_Angle - angle.m_Angle);
}
JGAngle2D& JGAngle2D::operator=(const float num)
{
	m_Angle = num;
	return *this;
}
JGAngle2D JGAngle2D::operator+(const float num)
{
	return JGAngle2D(m_Angle + num);
}
JGAngle2D JGAngle2D::operator-(const float num)
{
	return JGAngle2D(m_Angle - num);
}
bool JGAngle2D::operator==(JGAngle2D& angle)
{
	if (JGMath::Equal(m_Angle, angle.m_Angle))
	{
		return true;
	}
	return false;
}
bool JGAngle2D::operator!=(JGAngle2D& angle)
{
	bool result = this->operator==(angle);
	return !result;
}
bool JGAngle2D::operator<(JGAngle2D& angle)
{
	if (m_Angle < angle)
	{
		return true;
	}
	return false;
}
bool JGAngle2D::operator<=(JGAngle2D& angle)
{
	if (this->operator==(angle) || this->operator<(angle))
	{
		return true;
	}
	return false;
}
bool JGAngle2D::operator>(JGAngle2D& angle)
{
	if (m_Angle > angle.m_Angle)
	{
		return true;
	}
	return false;
}
bool JGAngle2D::operator>=(JGAngle2D& angle)
{
	if (this->operator==(angle) || this->operator>(angle))
	{
		return true;
	}
	return false;
}
bool JGAngle2D::operator==(const float num)
{
	if (JGMath::Equal(m_Angle, num))
	{
		return true;
	}
	return false;
}
bool JGAngle2D::operator!=(const float num)
{
	bool result = this->operator==(num);
	return !result;
}
bool JGAngle2D::operator<(const float num)
{
	if (m_Angle < num)
	{
		return true;
	}
	return false;
}
bool JGAngle2D::operator<=(const float num)
{
	if (this->operator==(num) || this->operator<(num))
	{
		return true;
	}
	return false;
}
bool JGAngle2D::operator>(const float num)
{
	if (m_Angle > num)
	{
		return true;
	}
	return false;
}
bool JGAngle2D::operator>=(const float num)
{
	if (this->operator==(num) || this->operator>(num))
	{
		return true;
	}
	return false;
}


JGAngle2D operator+(const float num, JGAngle2D& angle)
{
	return JGAngle2D(num + angle.m_Angle);
}
JGAngle2D operator-(const float num, JGAngle2D& angle)
{
	return JGAngle2D(num - angle.m_Angle);
}


JGAngle2D operator*(JGAngle2D& angle, const float num)
{
	return JGAngle2D(num * angle.m_Angle);
}
JGAngle2D operator*(const float num, JGAngle2D& angle)
{
	return JGAngle2D(num * angle.m_Angle);
}

bool operator==(const float num, JGAngle2D& angle)
{
	return angle.operator==(num);
}
bool operator!=(const float num, JGAngle2D& angle)
{
	return angle.operator!=(num);
}
bool operator<(const float num, JGAngle2D& angle)
{
	return angle.operator<(num);
}
bool operator<=(const float num, JGAngle2D& angle)
{
	return angle.operator<=(num);
}
bool operator>(const float num, JGAngle2D& angle)
{
	return angle.operator>(num);
}
bool operator>=(const float num, JGAngle2D& angle)
{
	return angle.operator>=(num);
}