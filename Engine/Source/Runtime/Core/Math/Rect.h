#pragma once
#include "Vector.h"
#include "String/String.h"

class PRect
{
public:
	float32 left   = 0.0f;
	float32 right  = 0.0f;
	float32 top    = 0.0f;
	float32 bottom = 0.0f;

public:
	PRect() noexcept = default;
	PRect(float32 left, float32 top, float32 right, float32 bottom) noexcept : left(left), right(right), top(top), bottom(bottom) {}
	PRect(const PVector2& min, const PVector2& max) noexcept : left(min.x), right(max.x), top(min.y), bottom(max.y) {}

	void Set(float32 left, float32 top, float32 right, float32 bottom) noexcept 
	{
		this->left = left; this->right = right; this->top = top; this->bottom = bottom;
	}

	void Set(const PVector2& min, const PVector2& max) noexcept 
	{
		this->left = min.x; this->right = max.x; this->top = min.y; this->bottom = max.y;
	}

	PVector2 Center() const noexcept 
	{
		return { Width() * 0.5f + left, Height() * 0.5f + top };
	}

	float32 Area()   const noexcept 
	{
		return Width() * Height();
	}

	float32 Width()  const noexcept 
	{
		return PMath::Abs(right - left);
	}

	float32 Height() const noexcept 
	{
		return PMath::Abs(bottom - top);
	}

	bool  Contain(const PRect& rect) const noexcept 
	{
		return Contain({ rect.left, rect.top }) && Contain({ rect.right, rect.bottom });
	}

	bool Contain(const PVector2& v) const
	{
		if (left <= v.x && right >= v.x && top <= v.y && bottom >= v.y) 
			return true;
		return false;
	}

	bool Contain(float32 x, float32 y) 
	{
		return Contain(PVector2(x, y));
	}

	bool  Overlap(const PRect& rect) const noexcept 
	{
		return Contain({ rect.left, rect.top }) || Contain({ rect.right, rect.bottom }) ||
			Contain({ rect.left, rect.bottom }) || Contain({ rect.right, rect.top });
	}

	bool Equal(const PRect& rect, float32 errorRange = ERROR_RANGE) const noexcept
	{
		if (PMath::Abs(rect.left - left)	 > errorRange) return false;
		if (PMath::Abs(rect.right - right)	 > errorRange) return false;
		if (PMath::Abs(rect.top - top)		 > errorRange) return false;
		if (PMath::Abs(rect.bottom - bottom) > errorRange) return false;
		return true;
	}

	PString ToString() const 
	{
		return PString::Format("[ %3.f, %3.f ][ %3.f, %3.f ]", left, top, right, bottom);	
	}
};
