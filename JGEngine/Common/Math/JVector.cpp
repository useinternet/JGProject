#include "pch.h"
#include "JVector.h"


JVector3 operator* (float k, const JVector3& v)
{
	return v * k;
}