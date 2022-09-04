#pragma once
#include "Vector.h"

class HPlane
{
public:
	HPlane() = default;
	HPlane(const HVector3& p1, const HVector3& p2, const HVector3& p3) noexcept;
	HPlane(const HVector3& p, const HVector3& N) noexcept;
	HPlane(const HVector3& N, float32 D) noexcept;

public:
	float Dot(const HVector4& v);

public:
	HVector3 N;
	float32 D = 0.0f;
};