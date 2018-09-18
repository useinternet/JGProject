#include"JGShape.h"

JGShape::JGShape()
{

}
JGShape::~JGShape()
{

}
b2Shape* JGShape::Get()
{
	return m_Shape;
}

void JGShape::RegisterShapeOrigin(b2Shape* shape)
{
	m_Shape = shape;
}
