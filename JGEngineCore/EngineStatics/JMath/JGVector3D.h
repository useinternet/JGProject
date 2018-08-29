#pragma once
#include"../Engine.h"

class JGMatrix;
class ENGINE_EXPORT JGVector3D
{
private:
	friend JGVector3D operator*(JGVector3D& vec, const float scalar);
	friend JGVector3D operator*(const float scalar, JGVector3D& vec);
private:
	D3DXVECTOR3 m_Vector3D = { 0.0f,0.0f,0.0f };

public:
	JGVector3D();
	JGVector3D(const float x, const float y, const float z);
	JGVector3D(const JGVector3D& copy);
	~JGVector3D();

	/*
	Exp : 이 벡터가 제로벡터인지 판별한다. */
	bool IsZeroVector();
	/*
	Exp : 내적을 구한다. */
	JGVector3D  Dot(JGVector3D& Vector);
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
	float Z();
	/*
	Exp : 이 벡터값 대입.*/
	void Set(const float x, const float y,const float z);
	/*
	Exp : 이 벡터값 대입.*/
	void Set(const JGVector3D& vec);


	// 행렬 연산용
	D3DXVECTOR3& Get();
	D3DXVECTOR3* GetAddress();
	// 연산자 오버로딩
	JGVector3D& operator=(JGVector3D& vec);
	JGVector3D operator+(JGVector3D& vec);
	JGVector3D operator-(JGVector3D& vec);
	bool operator==(JGVector3D& vec);
	bool operator!=(JGVector3D& vec);
	/*
	Exp : 행렬에 w값을 1로 설정하여 연산, 이동값이 적용... */
	void TransformCoord(JGMatrix& matrix);
	/*
	Exp : 행렬에 w값을 0으로 설정하여 연산, 이동값이 무시.*/
	void TransformNormal(JGMatrix& matrix);
};

// 연산자 오버로딩
JGVector3D operator*(JGVector3D& vec, const float scalar);
JGVector3D operator*(const float scalar, JGVector3D& vec);