#include"JGMath.h"
#include"JGVector2D.h"
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

float JGMath::Max(const float num1, const float num2)
{
	return num1 < num2 ? num2 : num1;
}

float JGMath::Min(const float num1, const float num2)
{
	return num1 < num2 ? num1: num2;
}

void JGMath::DrawRegion(float& target, const float min, const float max)
{
	if (target < min)
	{
		target = min;
	}
	else if (target > max)
	{
		target = max;
	}
}

JGVector2D JGMath::D2DToScreen(const float ScreenWidth, const float ScreenHeight, JGVector2D& vector)
{
	return JGVector2D(vector.X() - (ScreenWidth / 2.0f) ,
		              -vector.Y() + (ScreenHeight / 2.0f));
}

JGVector2D JGMath::D2DToScreen(const float ScreenWidth, const float ScreenHeight, const float x, const float y)
{
	return JGVector2D(x - (ScreenWidth / 2.0f),
		              -y + (ScreenHeight / 2.0f));
}
