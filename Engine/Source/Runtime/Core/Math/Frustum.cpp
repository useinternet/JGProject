#include "Frustum.h"

using namespace std;

PFrustum::PFrustum(const PMatrix& invViewProj) noexcept
{
	PVector4 frustumConer[8];

	frustumConer[0] = PVector4(-1, 1, 0, 1); // near_left_top
	frustumConer[1] = PVector4(1, 1, 0, 1); // near_right_top
	frustumConer[2] = PVector4(1, -1, 0, 1); // near_right_bottom
	frustumConer[3] = PVector4(-1, -1, 0, 1); // near_left_bottom 

	frustumConer[4] = PVector4(-1, 1, 1, 1); // far_left_top
	frustumConer[5] = PVector4(1, 1, 1, 1); // far_right_top
	frustumConer[6] = PVector4(1, -1, 1, 1); // far_right_bottom
	frustumConer[7] = PVector4(-1, -1, 1, 1); // far_left_bottom 

	for (int32 i = 0; i < 8; ++i)
	{
		frustumConer[i] = invViewProj.Transform(frustumConer[i]);
		frustumConer[i] /= frustumConer[i].w;
	}

	Plane[Near] = PPlane(frustumConer[0], frustumConer[3], frustumConer[2]);
	Plane[Far] = PPlane(frustumConer[4], frustumConer[5], frustumConer[6]);

	Plane[Top] = PPlane(frustumConer[4], frustumConer[0], frustumConer[1]);
	Plane[Bottom] = PPlane(frustumConer[7], frustumConer[6], frustumConer[2]);

	Plane[Left] = PPlane(frustumConer[7], frustumConer[3], frustumConer[0]);
	Plane[Right] = PPlane(frustumConer[5], frustumConer[1], frustumConer[2]);
}

bool PFrustum::IntersectBBox(const PBBox& b) const
{
	for (int32 i = 0; i < Count; ++i)
	{
		float32 minDot = Plane[i].Dot(PVector4(b.min.x, b.min.y, b.min.z, 1.0f));
		float32 maxDot = Plane[i].Dot(PVector4(b.max.x, b.max.y, b.max.z, 1.0f));

		if (minDot < 0.0f && maxDot < 0.0f)
		{
			return false;
		}
	}

	return true;
}
