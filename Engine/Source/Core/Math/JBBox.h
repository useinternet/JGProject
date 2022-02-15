#pragma once

#include "JVector.h"
#include "JRay.h"

namespace JG
{
	class JBBox
	{
	public:
		bool Contain(const JVector3& point) const {

			return
				min.x <= point.x && max.x >= point.x &&
				min.y <= point.y && max.y >= point.y &&
				min.z <= point.z && max.z >= point.z;
		}
		bool Intersection(const JRay& ray) const
		{
			float MinT = FLT_MAX;
			float MaxS = -FLT_MAX;
			JVector3 center = Center();
			for (int i = 0; i < 3; ++i)
			{
				if (std::abs(ray.dir.At(i)) <= ERROR_RANGE)
				{
					if (ray.origin.At(i) < min.At(i) || ray.origin.At(i) > max.At(i))
						return false;
				}
				else
				{
					float s = (min.At(i) - ray.origin.At(i)) / ray.dir.At(i);
					float t = (max.At(i) - ray.origin.At(i)) / ray.dir.At(i);
					if (s > t)
					{
						float temp = s;
						s = t;
						t = temp;
					}
					if (s > MaxS) MaxS = s;
					if (t < MinT) MinT = t;

					if (MinT < 0.0f || MaxS > MinT)
						return false;
				}
			}
			return true;
		}
		JVector3 Center() const {
			return 	(max + min) * 0.5f;
		}
	public:
		JVector3 min;
		JVector3 max;
	};
}
