#include"JGShape.h"
#include"../EngineStatics/JMath/JGVector2D.h"
#include"../EngineStatics/JMath/JGAngle2D.h"
#include"../RenderSystem/JGMaterial/JG2DMesh.h"
using namespace std;


JGShape::JGShape(EShapeType type)
{
	m_ShapeType = type;
	m_Location = make_unique<JGVector2D>();
	m_Angle = make_unique<JGAngle2D>();
}
JGShape::~JGShape()
{

}

void JGShape::Tick()
{


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