#pragma once
#include "Common/Define.h"
#define ERROR_RANGE 0.00000005f
#define JG_PI 3.14159265359f

#define JG_2PI 6.28318530718f

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
	};



}

