#include "pch.h"
#include "JVector.h"
#include "DataType/Color.h"

JVector4::JVector4(const JColor& c) :
	x(c.r), y(c.g), z(c.b), w(c.a) {}
JVector3 operator* (float k, const JVector3& v)
{
	return v * k;
}