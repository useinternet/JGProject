#pragma once

#include "Plane.h"
#include "BBox.h"
#include "Matrix.h"

class HFrustum
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


	mutable HPlane Plane[Count];

public:
	HFrustum() = default;
	HFrustum(const HMatrix& invViewProj) noexcept;

public:
	bool IntersectBBox(const HBBox& b) const;
};
