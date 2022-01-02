#pragma once
#include "Common/Define.h"
#define ERROR_RANGE 0.00000005f
#define JG_PI  3.14159265359f
#define JG_2PI 6.28318530718f

//Rad2Deg

namespace JG
{
	class JVector3;
	class Math
	{
	public:
		inline constexpr static float ConvertToRadians(float fDegrees) { return fDegrees * (JG_PI / 180.0f); }
		inline constexpr static float ConvertToDegrees(float fRadians) { return fRadians * (180.0f / JG_PI); }


		static JVector3 ConvertToRadians(const JVector3& vDegrees);
		static JVector3 ConvertToDegrees(const JVector3& vRadians);


		template<class T>
		inline static T Abs(T value)
		{
			if (value < 0)
			{
				value = value * -1;
			}
			return value;
		}

		template<class T>
		inline static T Max(const T& v1, const T& v2)
		{
			if (v1 < v2) {
				return v2;
			}
			else {
				return v1;
			}
		}
		 
		template<class T>
		inline static T Min(const T& v1, const T& v2)
		{
			if (v1 < v2) {
				return v1;
			}
			else {
				return v2;
			}
		}

        template<class T>
        inline static T Lerp(const T& A, const T& B, const T& Alpha)
        {
            return A * (1 - Alpha) + B * Alpha;
        }


        template <typename T> 
        inline static T AlignUpWithMask(T value, size_t mask)
        {
            return (T)(((size_t)value + mask) & ~mask);
        }

        template <typename T> 
        inline static T AlignDownWithMask(T value, size_t mask)
        {
            return (T)((size_t)value & ~mask);
        }

        template <typename T> 
        inline static T AlignUp(T value, size_t alignment)
        {
            return AlignUpWithMask(value, alignment - 1);
        }

        template <typename T> 
        inline static T AlignDown(T value, size_t alignment)
        {
            return AlignDownWithMask(value, alignment - 1);
        }

        template <typename T> 
        inline static bool IsAligned(T value, size_t alignment)
        {
            return 0 == ((size_t)value & (alignment - 1));
        }

        template <typename T> 
        inline static T DivideByMultiple(T value, size_t alignment)
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

        inline static u8 Log2(u64 value)
        {
            unsigned long mssb;
            unsigned long lssb; 

            if (_BitScanReverse64(&mssb, value) > 0 && _BitScanForward64(&lssb, value) > 0)
                return u8(mssb + (mssb == lssb ? 0 : 1));
            else
                return 0;
        }

        template <typename T> 
        inline T AlignPowerOfTwo(T value)
        {
            return value == 0 ? 0 : 1 << Log2(value);
        }
	};



}

