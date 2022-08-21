#include "Plane.h"
#include "Vector.h"
using namespace std;

PPlane::PPlane(const PVector3& p1, const PVector3& p2, const PVector3& p3) noexcept :
	PPlane(p1, PVector3::Cross(p2 - p1, p3 - p1)) {}

PPlane::PPlane(const PVector3& p, const PVector3& N) noexcept
{
	this->N = PVector3::Normalize(N);
	this->D = -PVector3::Dot(p, this->N);
}

PPlane::PPlane(const PVector3& N, float32 D) noexcept : N(N), D(D) {}

float32 PPlane::Dot(const PVector4& v)
{
	return (N.x * v.x + N.y * v.y + N.z * v.z + D * v.w);
}
