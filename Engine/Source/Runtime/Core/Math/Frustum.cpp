#include "PCH/PCH.h"
#include "Frustum.h"

using namespace std;

HFrustum::HFrustum(const HMatrix& invViewProj) noexcept
{
	HVector4 frustumConer[8];

	frustumConer[0] = HVector4(-1, 1, 0, 1); // near_left_top
	frustumConer[1] = HVector4(1, 1, 0, 1); // near_right_top
	frustumConer[2] = HVector4(1, -1, 0, 1); // near_right_bottom
	frustumConer[3] = HVector4(-1, -1, 0, 1); // near_left_bottom 

	frustumConer[4] = HVector4(-1, 1, 1, 1); // far_left_top
	frustumConer[5] = HVector4(1, 1, 1, 1); // far_right_top
	frustumConer[6] = HVector4(1, -1, 1, 1); // far_right_bottom
	frustumConer[7] = HVector4(-1, -1, 1, 1); // far_left_bottom 

	for (int32 i = 0; i < 8; ++i)
	{
		frustumConer[i] = invViewProj.Transform(frustumConer[i]);
		frustumConer[i] /= frustumConer[i].w;
	}

	Plane[Near] = HPlane(frustumConer[0], frustumConer[3], frustumConer[2]);
	Plane[Far] = HPlane(frustumConer[4], frustumConer[5], frustumConer[6]);

	Plane[Top] = HPlane(frustumConer[4], frustumConer[0], frustumConer[1]);
	Plane[Bottom] = HPlane(frustumConer[7], frustumConer[6], frustumConer[2]);

	Plane[Left] = HPlane(frustumConer[7], frustumConer[3], frustumConer[0]);
	Plane[Right] = HPlane(frustumConer[5], frustumConer[1], frustumConer[2]);
}

bool HFrustum::IntersectBBox(const HBBox& b) const
{
	for (int32 i = 0; i < Count; ++i)
	{
		float32 minDot = Plane[i].Dot(HVector4(b.min.x, b.min.y, b.min.z, 1.0f));
		float32 maxDot = Plane[i].Dot(HVector4(b.max.x, b.max.y, b.max.z, 1.0f));

		if (minDot < 0.0f && maxDot < 0.0f)
		{
			return false;
		}
	}

	return true;
}
