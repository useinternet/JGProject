#pragma once
#include<iostream>
#include<string>
#include<algorithm>
#include<DirectXMath.h>
#include<DirectXPackedVector.h>
namespace JGLibrary
{
#define ERROR_CONSTANT 0.0000001f
#define MACHINEEPSILON std::numeric_limits<float>::epsilon()
#define JG_PI 3.14159265358979323846f
#define JG_INV_PI 0.31830988618379067154f
#define JG_INV_2PI 0.15915494309189533577f
#define JG_INV_4PI 0.07957747154594766788f
#define PI_DIV_2  1.57079632679489661923f
#define PI_DIV_4  0.78539816339744830961f
#define PI_SQRT2  1.41421356237309504880f

	typedef unsigned int       jg_uint32;
	typedef unsigned long long jg_uint64;
	typedef DirectX::XMFLOAT2   xmfloat2;
	typedef DirectX::XMFLOAT3   xmfloat3;
	typedef DirectX::XMFLOAT4   xmfloat4;
	typedef DirectX::XMFLOAT4X4 xmfloat4x4;
	typedef DirectX::XMVECTOR   SimVector;
	typedef DirectX::FXMVECTOR  FSimVector;
	typedef DirectX::XMMATRIX   SimMatrix;
	typedef DirectX::FXMMATRIX  FSimMatrix;

	static float JGLIB_DUMMY = -1.0f;


	inline jg_uint32 FloatToBits(float f) {
		jg_uint32 ui;
		memcpy(&ui, &f, sizeof(float));
		return ui;
	}

	inline float BitsToFloat(jg_uint32 ui) {
		float f;
		memcpy(&f, &ui, sizeof(jg_uint32));
		return f;
	}

	inline float NextFloatUp(float v) {

		if (std::isinf(v) && v > 0.) return v;
		if (v == -0.f) v = 0.f;

		jg_uint32 ui = FloatToBits(v);
		if (v >= 0)
			++ui;
		else
			--ui;
		return BitsToFloat(ui);
	}

	inline float NextFloatDown(float v) {

		if (std::isinf(v) && v < 0.) return v;
		if (v == 0.f) v = -0.f;
		jg_uint32 ui = FloatToBits(v);
		if (v > 0)
			--ui;
		else
			++ui;
		return BitsToFloat(ui);
	}


	inline float Lerp(float v1, float v2, float t) { return (1 - t) * v1 + t * v2; }
	inline float Standard(float x) {
		(-ERROR_CONSTANT <= x || x <= ERROR_CONSTANT) ?
			x = 0.0f : x = x;
		return x;
	}
	inline float Gamma(int n) {
		return (n * MACHINEEPSILON) / (1 - n * MACHINEEPSILON);
	}

	template <typename T, typename U, typename V>
	inline T Clamp(T val, U low, V high) {
		if (val < low)
			return (T)low;
		else if (val > high)
			return (T)high;
		else
			return (T)val;
	}
}