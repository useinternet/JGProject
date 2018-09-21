#pragma once
#include"../Engine.h"
/*
Class : JMath 
Exp : 수학 관련 연산 함수들은 다 여기에~~!*/
class JGVector2D;
class ENGINE_EXPORT JGMath
{
public:
	static const float ErrorRange;
public:
	/*
	Exp : 제곱근을 구한다.*/
	static float Sqrt(float num);
	/*
	Exp : 값이 제로인지 판별한다.(오차범위안에서) */
	static bool IsZero(const float num);
	/*
	Exp : 두개의 값이 같은지 판별한다.(오차범위 안에서)*/
	static bool Equal(const float num1,const float num2);
	/*
	Exp : 절대값을 구한다. */
	static float Abs(const float num);
	/*
	Exp : 최대값 을 구한다.  */
	static float Max(const float num1, const float num2);
	/*
	Exp : 최솟값을 구한다. */
	static float Min(const float num1, const float num2);
	/*
	Exp : 범위를 정한다.. */
	static void DrawRegion(float& target,const float min, const float max);
	/*
	현제 Direct 2D 좌표계를 스크린 좌표계로 바꾼다. */
	static JGVector2D D2DToScreen(const float ScreenWidth, const float ScreenHeight,JGVector2D& vector);
	static JGVector2D D2DToScreen(const float ScreenWidth, const float ScreenHeight,const float x, const float y);
};