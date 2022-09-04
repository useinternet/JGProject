#pragma once
#include "CoreDefines.h"

class HColor
{
public:
	uint8 R, G, B, A;
};

class HLinearColor
{
public:
	float32 R, G, B, A;

	HLinearColor(float32 r = 0.0f, float32 g = 0.0f, float32 b = 0.0f, float32 a = 0.0f)
		: R(r)
		, G(g)
		, B(b)
		, A(a) {}
};