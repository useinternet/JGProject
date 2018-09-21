#include"JGCircle.h"
#include"../JGCommon/b2ToJGCommon.h"
using namespace std;
JGCircle::JGCircle()
{
	m_Circle = make_unique<b2CircleShape>();

	RegisterShapeOrigin(m_Circle.get());
}

JGCircle::~JGCircle()
{
}

void JGCircle::SetCircle(const float Radius)
{

	m_Circle->m_radius = Radius / PTM;
}
