#include "pch.h"
#include "JPlane.h"
#include "JVector.h"
using namespace std;


namespace JG
{
	JPlane::JPlane(const JVector3& p1, const JVector3& p2, const JVector3& p3) noexcept :
		JPlane(p1, JVector3::Cross(p2 - p1, p3 - p1)) {}

	JPlane::JPlane(const JVector3& p, const JVector3& N) noexcept
	{
		this->N = JVector3::Normalize(N);
		this->D = -JVector3::Dot(p, this->N);
	}

	JPlane::JPlane(const JVector3& N, float D) noexcept : N(N), D(D) {}

	float JPlane::Dot(const JVector4& v)
	{
		return (N.x * v.x + N.y * v.y + N.z * v.z + D * v.w);
	}
}

