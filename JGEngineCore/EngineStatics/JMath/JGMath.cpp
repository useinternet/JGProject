#include"JGMath.h"

const float JGMath::ErrorRange = 0.00000005f;

float JGMath::Sqrt(float num)
{
	float x = 1;
	while (true)
	{
		x = (x + num / x) / 2;
		if (num - ErrorRange < x*x && x*x < num + ErrorRange)
		{
			break;
		}
	}
	return x;
}

bool JGMath::IsZero(const float num)
{
	if (-ErrorRange < num && num < ErrorRange)
	{
		return true;
	}
	return false;
}

bool JGMath::Equal(const float num1, const float num2)
{
	float value = Abs(Abs(num1) - Abs(num2));
	if (value < ErrorRange)
	{
		return true;
	}
	return false;
}

float JGMath::Abs(const float num)
{
	if (num < 0)
		return -num;
	else
		return num;
}
