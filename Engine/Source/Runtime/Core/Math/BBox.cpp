#include "PCH/PCH.h"
#include "BBox.h"
#include "Ray.h"

bool HBBox::Contain(const HVector3& point) const
{
	return
		min.x <= point.x && max.x >= point.x &&
		min.y <= point.y && max.y >= point.y &&
		min.z <= point.z && max.z >= point.z;
}

bool HBBox::Intersection(const HRay& ray) const
{
	float32 MinT = JG_FLOAT32_MAX;
	float32 MaxS = JG_FLOAT32_MIN;
	HVector3 center = Center();

	for (int32 i = 0; i < 3; ++i)
	{
		if (HMath::Abs(ray.dir.At(i)) <= ERROR_RANGE)
		{
			if (ray.origin.At(i) < min.At(i) || ray.origin.At(i) > max.At(i))
				return false;
		}
		else
		{
			float32 s = (min.At(i) - ray.origin.At(i)) / ray.dir.At(i);
			float32 t = (max.At(i) - ray.origin.At(i)) / ray.dir.At(i);
			if (s > t)
			{
				float32 temp = s;
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

HVector3 HBBox::Center() const
{
	return 	(max + min) * 0.5f;
}