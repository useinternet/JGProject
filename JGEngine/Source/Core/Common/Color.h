#pragma once
#include "Define.h"
#include "Math/JVector.h"




namespace JG
{
	class Color
	{
	public:
		f32 R = 0.0f;
		f32 G = 0.0f;
		f32 B = 0.0f;
		f32 A = 0.0F;
	public:
		Color() = default;
		Color(f32 r, f32 g, f32 b, f32 a) : R(r), G(g), B(b), A(a) {}
		Color(const JVector4& v) : R(v.x), G(v.y), B(v.z), A(v.w) {}
	public:
		static Color White() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
		static Color Black() { return Color(0.0f,0.0f,0.0f,1.0f); }
		static Color Red()   { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
		static Color Blue()  { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
		static Color Green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
	public:
		bool operator==(const Color& c) {
			return R == c.R && G == c.G && B == c.B && A == c.A;
		}
		bool operator!=(const Color& c) {
			return !operator==(c);
		}
	};
}