#pragma once
#include "Vector.h"
#include "String/String.h"

class HRect
{
public:
	float32 left   = 0.0f;
	float32 right  = 0.0f;
	float32 top    = 0.0f;
	float32 bottom = 0.0f;

public:
	HRect() noexcept = default;
	HRect(float32 left, float32 top, float32 right, float32 bottom) noexcept : left(left), right(right), top(top), bottom(bottom) {}
	HRect(const HVector2& min, const HVector2& max) noexcept : left(min.x), right(max.x), top(min.y), bottom(max.y) {}

	void Set(float32 left, float32 top, float32 right, float32 bottom) noexcept 
	{
		this->left = left; this->right = right; this->top = top; this->bottom = bottom;
	}

	void Set(const HVector2& min, const HVector2& max) noexcept
	{
		this->left = min.x; this->right = max.x; this->top = min.y; this->bottom = max.y;
	}

	HVector2 Min() const noexcept
	{
		return HVector2(left, top);
	}

	HVector2 Max() const noexcept
	{
		return HVector2(right, bottom);
	}

	HVector2 Center() const noexcept 
	{
		return { Width() * 0.5f + left, Height() * 0.5f + top };
	}

	float32 Area()   const noexcept 
	{
		return Width() * Height();
	}

	float32 Width()  const noexcept 
	{
		return HMath::Abs(right - left);
	}

	float32 Height() const noexcept 
	{
		return HMath::Abs(bottom - top);
	}

	bool  Contain(const HRect& rect) const noexcept 
	{
		return Contain({ rect.left, rect.top }) && Contain({ rect.right, rect.bottom });
	}

	bool Contain(const HVector2& v) const
	{
		if (left <= v.x && right >= v.x && top <= v.y && bottom >= v.y) 
			return true;
		return false;
	}

	bool Contain(float32 x, float32 y) 
	{
		return Contain(HVector2(x, y));
	}

	bool  Overlap(const HRect& rect) const noexcept 
	{
		return Contain({ rect.left, rect.top }) || Contain({ rect.right, rect.bottom }) ||
			Contain({ rect.left, rect.bottom }) || Contain({ rect.right, rect.top });
	}

	bool Equal(const HRect& rect, float32 errorRange = ERROR_RANGE) const noexcept
	{
		if (HMath::Abs(rect.left - left)	 > errorRange) return false;
		if (HMath::Abs(rect.right - right)	 > errorRange) return false;
		if (HMath::Abs(rect.top - top)		 > errorRange) return false;
		if (HMath::Abs(rect.bottom - bottom) > errorRange) return false;
		return true;
	}

	PString ToString() const 
	{
		return PString::Format("[ %3.f, %3.f ][ %3.f, %3.f ]", left, top, right, bottom);	
	}
};
