#include"JGShape.h"
#include"../../EngineStatics/JMath/JGVector2D.h"
#include"../../EngineStatics/JMath/JGAngle2D.h"
#include"../../RenderSystem/JGMaterial/JG2DMesh.h"
using namespace std;


JGShape::JGShape(EShapeType type, JGVector2D* pVector, JGAngle2D* pAngle)
{
	m_ShapeType = type;
	m_Location = pVector;
	m_Angle = pAngle;
	m_PrevLocation = make_unique<JGVector2D>();
	m_PrevAngle = make_unique<JGAngle2D>();
}
JGShape::~JGShape()
{

}

void JGShape::Tick()
{
	if (m_bIsTransLocation)
	{
		*m_PrevLocation = *m_Location;
	}
	if(m_bIsTransAngle)
	{
		*m_PrevAngle = *m_Angle;
	}

	m_bIsTransLocation = false;
	m_bIsTransAngle = false;

	if (*m_PrevLocation != *m_Location)
	{
		m_bIsTransLocation = true;
	}
	if (*m_PrevAngle != *m_Angle)
	{	
		m_bIsTransAngle = true;
	}
}
EShapeType JGShape::GetShapeType()
{
	return m_ShapeType;
}
JGVector2D& JGShape::GetLocation()
{
	return *m_Location;
}
JGAngle2D& JGShape::GetAngle()
{
	return *m_Angle;
}
JGVector2D& JGShape::GetPrevLocation()
{
	return *m_PrevLocation;
}
JGAngle2D& JGShape::GetPrevAngle()
{
	return *m_PrevAngle;
}
bool JGShape::IsTransLocation()
{
	return m_bIsTransLocation;
}
bool JGShape::IsTransAngle()
{
	return m_bIsTransAngle;
}
void JGShape::AddLocation(JGVector2D& vec)
{
	*m_Location += vec;
}
void JGShape::AddAngle(JGAngle2D& angle)
{
	*m_Angle += angle;
}
void JGShape::SetLocation(const JGVector2D& vec)
{
	m_Location->Set(vec);
}
void JGShape::SetAngle(const JGAngle2D& angle)
{
	m_Angle->Set(angle);
}

bool JGShape::IsValue()
{
	if (m_Location && m_Angle)
	{
		return true;
	}
	return false;
}
