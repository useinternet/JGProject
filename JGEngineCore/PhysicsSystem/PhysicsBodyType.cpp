#include"PhysicsBodyType.h"
#include"../EngineStatics/JMath/JGVector2D.h"
Figure::~Figure()
{
}

Box::~Box()
{
}
void Box::Set(const float top, const float left, const float right, const float bottom)
{
	Top = top;
	Left = left;
	Right = right;
	Bottom = bottom;
}
void Box::Set(JGVector2D& LeftTop, JGVector2D& RightBottom)
{
	Left = LeftTop.X();
	Top = LeftTop.Y();
	Right = RightBottom.X();
	Bottom = RightBottom.Y();
}
Circle::~Circle()
{
}

Capsule::~Capsule()
{
}
