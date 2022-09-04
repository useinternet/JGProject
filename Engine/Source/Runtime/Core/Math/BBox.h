#pragma once
#include "Math.h"
#include "Vector.h"

class HRay;
class HBBox
{
public:
	HVector3 min;
	HVector3 max;

public:
	bool Contain(const HVector3& point) const;

	bool Intersection(const HRay& ray) const;

	HVector3 Center() const;
};
