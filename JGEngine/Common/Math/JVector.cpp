#include "pch.h"
#include "JVector.h"
#include "DataType/Color.h"

JVector4::JVector4(const JColor& c) :
	x(c.r), y(c.g), z(c.b), w(c.a) {}

JVector2 operator* (float k, const JVector2& v)
{
	return v * k;
}
JVector3 operator* (float k, const JVector3& v)
{
	return v * k;
}
