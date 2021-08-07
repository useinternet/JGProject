#pragma once

#include "JVector.h"




namespace JG
{
	class JRect
	{
	public:
		float left = 0.0f;
		float right = 0.0f;
		float top = 0.0f;
		float bottom = 0.0f;
	public:
		JRect() noexcept = default;
		JRect(float left, float top, float right, float bottom) noexcept : left(left), right(right), top(top), bottom(bottom) {}
		JRect(const JVector2& min, const JVector2& max) noexcept : left(min.x), right(max.x), top(min.y), bottom(max.y) {}


		void Set(float left, float top, float right, float bottom) noexcept {
			this->left = left; this->right = right; this->top = top; this->bottom = bottom;
		}
		void Set(const JVector2& min, const JVector2& max) noexcept {
			this->left = min.x; this->right = max.x; this->top = min.y; this->bottom = max.y;
		}


		JVector2 Center() const noexcept {
			return { Width() * 0.5f + left, Height() * 0.5f + top };
		}

		float Area()   const noexcept {
			return Width() * Height();
		}
		float Width()  const noexcept {
			return std::abs(right - left);
		}
		float Height() const noexcept {
			return std::abs(bottom - top);
		}
		bool  Contain(const JRect& rect) const noexcept {
			// TODO 
			return Contain({ rect.left, rect.top }) && Contain({ rect.right, rect.bottom });
		}
		bool Contain(const JVector2& v) const
		{
			if (left <= v.x && right >= v.x && top <= v.y && bottom >= v.y) return true;
			return false;
		}
		bool Contain(float x, float y) {
			return Contain(JVector2(x, y));
		}
		bool  Overlap(const JRect& rect) const noexcept {
			//TODO
			return Contain({ rect.left, rect.top }) || Contain({ rect.right, rect.bottom }) ||
				Contain({ rect.left, rect.bottom }) || Contain({ rect.right, rect.top });
		}
		bool Equal(const JRect& rect, float errorRange = ERROR_RANGE) const noexcept
		{
			if (std::abs(rect.left - left) > errorRange)     return false;
			if (std::abs(rect.right - right) > errorRange)   return false;
			if (std::abs(rect.top - top) > errorRange)       return false;
			if (std::abs(rect.bottom - bottom) > errorRange) return false;
			return true;
		}
		String ToString() const {
			return "[" + std::to_string(left) + ", " + std::to_string(top) + "], [" + std::to_string(right) + ", " + std::to_string(bottom) + "]";
		}

	};
}
