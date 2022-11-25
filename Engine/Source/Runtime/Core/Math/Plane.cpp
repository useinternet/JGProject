#include "PCH/PCH.h"
#include "Plane.h"
#include "Vector.h"
using namespace std;

HPlane::HPlane(const HVector3& p1, const HVector3& p2, const HVector3& p3) noexcept :
	HPlane(p1, HVector3::Cross(p2 - p1, p3 - p1)) {}

HPlane::HPlane(const HVector3& p, const HVector3& N) noexcept
{
	this->N = HVector3::Normalize(N);
	this->D = -HVector3::Dot(p, this->N);
}

HPlane::HPlane(const HVector3& N, float32 D) noexcept : N(N), D(D) {}

float32 HPlane::Dot(const HVector4& v)
{
	return (N.x * v.x + N.y * v.y + N.z * v.z + D * v.w);
}
