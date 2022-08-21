#pragma once
#include "Math.h"
#include "Vector.h"

class PRay;
class PBBox
{
public:
	PVector3 min;
	PVector3 max;

public:
	bool Contain(const PVector3& point) const;

	bool Intersection(const PRay& ray) const;

	PVector3 Center() const;
};
