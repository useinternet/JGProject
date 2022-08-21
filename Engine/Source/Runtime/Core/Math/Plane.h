#pragma once
#include "Vector.h"

class PPlane
{
public:
	PPlane() = default;
	PPlane(const PVector3& p1, const PVector3& p2, const PVector3& p3) noexcept;
	PPlane(const PVector3& p, const PVector3& N) noexcept;
	PPlane(const PVector3& N, float32 D) noexcept;

public:
	float Dot(const PVector4& v);

public:
	PVector3 N;
	float32 D = 0.0f;
};