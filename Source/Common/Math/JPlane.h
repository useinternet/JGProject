#pragma once
#include "JVector.h"

class JPlane
{
public:
	JPlane() = default;
	JPlane(const JVector3& p1, const JVector3& p2, const JVector3& p3) noexcept;
	JPlane(const JVector3& p, const JVector3& N) noexcept;
	JPlane(const JVector3& N, float D) noexcept;
public:
	float Dot(const JVector4& v);
public:
	JVector3 N;
	float D = 0.0f;
};