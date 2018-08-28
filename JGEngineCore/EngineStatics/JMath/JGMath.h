#pragma once

/*
Class : JMath 
Exp : 수학 관련 연산 함수들은 다 여기에~~!*/
class JGMath
{
private:
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

};