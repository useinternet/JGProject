#pragma once

#include "Plane.h"
#include "BBox.h"
#include "Matrix.h"

class PFrustum
{
public:
	enum
	{
		Near = 0,
		Far = 1,
		Left = 2,
		Right = 3,
		Top = 4,
		Bottom = 5,
		Count = 6
	};


	mutable PPlane Plane[Count];

public:
	PFrustum() = default;
	PFrustum(const PMatrix& invViewProj) noexcept;

public:
	bool IntersectBBox(const PBBox& b) const;
};
