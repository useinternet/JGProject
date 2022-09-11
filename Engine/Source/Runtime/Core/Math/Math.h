#pragma once


#include "CoreDefines.h"

#define ERROR_RANGE 0.00000005f
#define JG_PI  3.14159265359f
#define JG_2PI 6.28318530718f

class HVector3;
class HMath
{
public:
	constexpr static float32 ConvertToRadians(float32 fDegrees) { return fDegrees * (JG_PI / 180.0f); }
	constexpr static float32 ConvertToDegrees(float32 fRadians) { return fRadians * (180.0f / JG_PI); }

	static HVector3 ConvertToRadians(const HVector3& vDegrees);
	static HVector3 ConvertToDegrees(const HVector3& vRadians);

	template<class T>
	static T Abs(T value)
	{
		if (value < 0)
		{
			value = value * -1;
		}
		return value;
	}

	template<class T>
	static T Max(const T& v1, const T& v2)
	{
		if (v1 < v2) {
			return v2;
		}
		else {
			return v1;
		}
	}

	template<class T>
	static T Min(const T& v1, const T& v2)
	{
		if (v1 < v2) {
			return v1;
		}
		else {
			return v2;
		}
	}

	template<class T>
	static T Lerp(const T& A, const T& B, const T& Alpha)
	{
		return A * (1 - Alpha) + B * Alpha;
	}

	template<class T>
	static T Clamp(T a, T _min, T _max)
	{
		return Max<T>(_min, Min<T>(_max, a));
	}

	template<class T>
	static T Saturate(T a)
	{
		return Clamp(a, 0, 1);
	}

	template<typename T>
	static T RelativeCoef(T a, T _min, T _max)
	{
		float _a = Clamp(a, _min, _max);
		return (_a - _min) / (_max - _min);
	}

	template <typename T>
	inline static T AlignUpWithMask(T value, uint64 mask)
	{
		return (T)(((uint64)value + mask) & ~mask);
	}

	template <typename T>
	inline static T AlignDownWithMask(T value, uint64 mask)
	{
		return (T)((uint64)value & ~mask);
	}

	template <typename T>
	inline static T AlignUp(T value, uint64 alignment)
	{
		return AlignUpWithMask(value, alignment - 1);
	}

	template <typename T>
	inline static T AlignDown(T value, uint64 alignment)
	{
		return AlignDownWithMask(value, alignment - 1);
	}

	template <typename T>
	inline static bool IsAligned(T value, uint64 alignment)
	{
		return 0 == ((uint64)value & (alignment - 1));
	}

	template <typename T>
	inline static T DivideByMultiple(T value, uint64 alignment)
	{
		return (T)((value + alignment - 1) / alignment);
	}

	template <typename T>
	inline static bool IsPowerOfTwo(T value)
	{
		return 0 == (value & (value - 1));
	}

	template <typename T>
	inline static bool IsDivisible(T value, T divisor)
	{
		return (value / divisor) * divisor == value;
	}

	inline static uint8 Log2(uint64 value)
	{
		unsigned long mssb;
		unsigned long lssb;

		if (_BitScanReverse64(&mssb, value) > 0 && _BitScanForward64(&lssb, value) > 0)
			return uint8(mssb + (mssb == lssb ? 0 : 1));
		else
			return 0;
	}

	template <typename T>
	inline T AlignPowerOfTwo(T value)
	{
		return value == 0 ? 0 : 1 << Log2(value);
	}

	template<class T>
	static T Align(T value, T alignment)
	{
		T mask = alignment - 1;

		return value + (-value & mask);
	}

	static float64 RoundToFloat64(float64 f)
	{
		return std::round(f);
	}

	static float64 CeilToFloat64(float64 f)
	{
		return std::ceil(f);
	}

	static float64 FloorToFloat64(float64 f)
	{
		return std::floor(f);
	}

	static float32 RoundToFloat32(float32 f)
	{
		return std::roundf(f);
	}

	static float32 CeilToFloat32(float32 f)
	{
		return std::ceilf(f);
	}

	static float32 FloorToFloat32(float32 f)
	{
		return std::floorf(f);
	}
	
	static int32 RoundToInt(float32 f)
	{
		return (int32)RoundToFloat32(f);
	}

	static int32 CeilToInt(float32 f)
	{
		return (int32)CeilToFloat32(f);
	}

	static int32 FloorToInt(float32 f)
	{
		return (int32)FloorToFloat32(f);
	}
};