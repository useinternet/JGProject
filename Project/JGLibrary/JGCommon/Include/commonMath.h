#pragma once
#include"common.h"


namespace JGCommon
{
    class JGMath
	{
	public:
		inline static real zero_standard(const real n, const real errorRange = ERRORRANGE) noexcept
		{
			if (ABS(n) <= errorRange)
			{
				return 0;
			}
			return n;
		}
		inline static bool is_zero(const real n, const real errorRange = ERRORRANGE) noexcept
		{
			if (ABS(n) <= errorRange)
			{
				return true;
			}
			return false;
		}
	};
}