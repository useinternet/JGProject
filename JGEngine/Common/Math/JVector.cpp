#include "pch.h"
#include "JVector.h"


JVector operator* (float k, const JVector& v)
{
	return v * k;
}