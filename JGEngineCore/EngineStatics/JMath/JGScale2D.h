#pragma once
#include"../Engine.h"

class JGScale2D
{
private:
	friend JGScale2D operator*(JGScale2D& vec, const float scalar);
	friend JGScale2D operator*(const float scalar, JGScale2D& vec);
private:
	D3DXVECTOR3 m_Scale2D = { 1.0f,1.0f,1.0f };
public:
	JGScale2D();
	JGScale2D(const float flatNum);
	JGScale2D(const float x, const float y);
	JGScale2D(const JGScale2D& copy);
	~JGScale2D();
	// x,y값 반환
	float X();
	float Y();
	/*
	Exp : x값 y값 모두 flatNum으로 대입한다. */
	void Set(const float flatNum);
	/*
	Exp : x값 y값 셋팅 */
	void Set(const float x, const float y);
	/*
	Exp : x값 y값 셋팅 */
	void Set(const JGScale2D& scale);
	/*
	Exp : 이 스켈이 제로값인지 판별한다. */
	bool IsZeroScale();

	// 오버로딩
	JGScale2D& operator=(JGScale2D& scale);
	JGScale2D operator+(JGScale2D& scale);
	JGScale2D operator-(JGScale2D& scale);
	bool operator==(JGScale2D& scale);
	bool operator!=(JGScale2D& scale);

};
// 오버로딩
JGScale2D operator*(JGScale2D& scale, const float scalar);
JGScale2D operator*(const float scalar, JGScale2D& scale);