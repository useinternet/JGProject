#pragma once

#include "JPlane.h"
#include "JBBox.h"
#include "JMatrix.h"

namespace JG
{
	class JFrustum
	{
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
	public:
		JFrustum() = default;
		JFrustum(const JMatrix& invViewProj) noexcept;
	public:
		bool IntersectBBox(const JBBox& b) const;
	public:
		mutable JPlane Plane[Count];

	};
}
