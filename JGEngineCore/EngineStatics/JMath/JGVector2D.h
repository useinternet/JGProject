#pragma once
#include"../Engine.h"
class JGMatrix;
/*
class : JGVector2D */
class ENGINE_EXPORT JGVector2D
{
private:
	friend JGVector2D operator*(JGVector2D& vec, const float scalar);
	friend JGVector2D operator*(const float scalar, JGVector2D& vec);
private:
	D3DXVECTOR3 m_Vector2D = { 0.0f,0.0f,0.0f };

public:
	JGVector2D();
	JGVector2D(const float x, const float y);
	JGVector2D(const JGVector2D& copy);
	~JGVector2D();

	/*
	Exp : 이 벡터가 제로벡터인지 판별한다. */
	bool IsZeroVector();
	/*
	Exp : 내적을 구한다. */
	JGVector2D  Dot(JGVector2D& Vector);
	/*
	Exp : 이 벡터를 정규화한다.*/
	void  Normalize();
	/*
	Exp : 이 벡터를 길이를 구한다.*/
	float Length();
	/*
	Exp : 이 벡터를 길이를 제곱한 상태 그대로 구한다.*/
	float LengthSquard();
	// x,y값 반환
	float X();
	float Y();
	/*
	Exp : 이 벡터값 대입.*/
	void Set(const float x, const float y);
	/*
	Exp : 이 벡터값 대입.*/
	void Set(const JGVector2D& vec);

	// 연산자 오버로딩
	JGVector2D& operator=(JGVector2D& vec);
	JGVector2D operator+(JGVector2D& vec);
	JGVector2D operator-(JGVector2D& vec);
	bool operator==(JGVector2D& vec);
	bool operator!=(JGVector2D& vec);

	//
	void TransformCoord(JGMatrix& matrix);
	void TransformNormal(JGMatrix& matrix);
};
// 연산자 오버로딩
JGVector2D operator*(JGVector2D& vec, const float scalar);
JGVector2D operator*(const float scalar, JGVector2D& vec);

