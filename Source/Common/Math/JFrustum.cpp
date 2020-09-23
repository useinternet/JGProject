#include "pch.h"
#include "JFrustum.h"

using namespace std;

JFrustum::JFrustum(const JMatrix& invViewProj) noexcept
{
	JVector4 frustumConer[8];

	frustumConer[0] = JVector4(-1,  1, 0, 1); // near_left_top
	frustumConer[1] = JVector4( 1,  1, 0, 1); // near_right_top
	frustumConer[2] = JVector4( 1, -1, 0, 1); // near_right_bottom
	frustumConer[3] = JVector4(-1, -1, 0, 1); // near_left_bottom 

	frustumConer[4] = JVector4(-1,  1, 1, 1); // far_left_top
	frustumConer[5] = JVector4( 1,  1, 1, 1); // far_right_top
	frustumConer[6] = JVector4( 1, -1, 1, 1); // far_right_bottom
	frustumConer[7] = JVector4(-1, -1, 1, 1); // far_left_bottom 


	for (int i = 0; i < 8; ++i)
	{
		frustumConer[i] = invViewProj.Transform(frustumConer[i]);
		frustumConer[i] /= frustumConer[i].w;
	}


	Plane[Near] = JPlane(frustumConer[0], frustumConer[3], frustumConer[2]); 
	Plane[Far] =  JPlane(frustumConer[4], frustumConer[5], frustumConer[6]);

	Plane[Top] =    JPlane(frustumConer[4], frustumConer[0], frustumConer[1]);
	Plane[Bottom] = JPlane(frustumConer[7], frustumConer[6], frustumConer[2]);

	Plane[Left] = JPlane(frustumConer[7], frustumConer[3], frustumConer[0]);
	Plane[Right] = JPlane(frustumConer[5], frustumConer[1], frustumConer[2]);
}

bool JFrustum::IntersectBBox(const JBBox& b) const
{
	// min // max
	for (int i = 0; i < Count; ++i)
	{
		float minDot = Plane[i].Dot(JVector4(b.min.x, b.min.y, b.min.z,1.0f));
		float maxDot = Plane[i].Dot(JVector4(b.max.x, b.max.y, b.max.z, 1.0f));
		if (minDot < 0.0f && maxDot < 0.0f)
		{
			return false;
		}
	}

	return true;
}
