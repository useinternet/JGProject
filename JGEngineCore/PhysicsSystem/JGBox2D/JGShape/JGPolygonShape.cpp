#include"JGPolygonShape.h"
#include"../../../EngineStatics/JMath/JGVector2D.h"
#include"../JGCommon/b2ToJGCommon.h"
using namespace std;
JGPolygonShape::JGPolygonShape()
{
	m_Shape = make_unique<b2PolygonShape>();
	RegisterShapeOrigin(m_Shape.get());
}

JGPolygonShape::~JGPolygonShape()
{
}

void JGPolygonShape::SetAsBox(const float HalfWidth, const float HalfHeight)
{
	JGVector2D vec(HalfWidth, HalfHeight);
	b2Vec2 Box2D_vec = JGTob2_Unit(vec);
	m_Shape->SetAsBox(Box2D_vec.x, Box2D_vec.y);
}
