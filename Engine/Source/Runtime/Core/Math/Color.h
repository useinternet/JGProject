#pragma once
#include "CoreDefines.h"

class PColor
{
public:
	uint8 R, G, B, A;
};

class PLinearColor
{
public:
	float32 R, G, B, A;

	PLinearColor(float32 r = 0.0f, float32 g = 0.0f, float32 b = 0.0f, float32 a = 0.0f)
		: R(r)
		, G(g)
		, B(b)
		, A(a) {}
};