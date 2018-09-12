#include"JGRectangle.h"
#include"../EngineStatics/JMath/JGVector2D.h"
using namespace std;
JGRectangle::JGRectangle(EShapeType type) : JGShape(type)
{
	m_LeftTop = make_unique<JGVector2D>();
	m_RightBottom = make_unique<JGVector2D>();
	m_PrevLocation = make_unique<JGVector2D>();
}

JGRectangle::~JGRectangle()
{

}

void JGRectangle::Tick()
{
	JGShape::Tick();

	if (*m_PrevLocation != GetLocation())
	{
		*m_PrevLocation = GetLocation();
		SetRectangle(m_half_Width, m_half_Height);
	}
}

void JGRectangle::SetRectangle(const float half_Width, const float half_Height)
{
	m_half_Width = half_Width;
	m_half_Height = half_Height;
	m_LeftTop->Set(
		GetLocation().X() - half_Width, GetLocation().Y() - half_Height
	);
	m_RightBottom->Set(
		GetLocation().X() + half_Width, GetLocation().Y() + half_Height
	);
}
JGVector2D& JGRectangle::GetLeftTop()
{
	return *m_LeftTop;
}
JGVector2D& JGRectangle::GetRightBottom()
{
	return *m_RightBottom;
}
float JGRectangle::GetHalfWidth()
{
	return m_half_Width;
}
float JGRectangle::GetHalfHeight()
{
	return m_half_Height;
}