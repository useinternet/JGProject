#pragma once
#include "CoreDefines.h"

class HViewport
{
public:
	float32 TopLeftX = 0.0f;
	float32 TopLeftY = 0.0f;
	float32 Width = 0.0f;
	float32 Height = 0.0f;
	float32 MinDepth = 0.0f;
	float32 MaxDepth = 1.0f;
public:
	HViewport(float32 w, float32 h) : Width(w), Height(h) {}
	HViewport(float32 topLeftX, float32 topLeftY, float32 w, float32 h) {}
};

class HScissorRect
{
public:
	int32    Left = 0;
	int32    Top = 0;
	int32    Right = 0;
	int32    Bottom = 0;

public:
	HScissorRect(int32 left, int32 top, int32 right, int32 bottom) :
		Left(left), Top(top), Right(right), Bottom(bottom) {}
};