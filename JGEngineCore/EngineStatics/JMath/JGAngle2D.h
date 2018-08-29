#pragma once
#include"../Engine.h"

class ENGINE_EXPORT JGAngle2D
{
private:
	friend JGAngle2D operator+(const float num, JGAngle2D& angle);
	friend JGAngle2D operator-(const float num, JGAngle2D& angle);
	friend JGAngle2D operator*(JGAngle2D& scale, const float scalar);
	friend JGAngle2D operator*(const float scalar, JGAngle2D& scale);
	friend bool operator==(const float num, JGAngle2D& angle);
	friend bool operator!=(const float num, JGAngle2D& angle);
	friend bool operator<(const float num, JGAngle2D& angle);
	friend bool operator<=(const float num, JGAngle2D& angle);
	friend bool operator>(const float num, JGAngle2D& angle);
	friend bool operator>=(const float num, JGAngle2D& angle);
private:
	float m_Angle = 0.0f;

public:
	JGAngle2D();
	JGAngle2D(float angle);
	JGAngle2D(const JGAngle2D& Angle);
	~JGAngle2D();

	float Get();
	void  Set(const float Angle);
	void  Set(const JGAngle2D& Angle);
	/*
	Exp : 360도보다 높거나 -360도보다 작으면 0도기준으로 다시 되돌린다. */
	void  StartState();


	// 오버로딩
	JGAngle2D& operator++();
	JGAngle2D operator++(const int num);

	JGAngle2D& operator+=(JGAngle2D& angle);
	JGAngle2D& operator-=(JGAngle2D& angle);
	JGAngle2D& operator+=(const float num);
	JGAngle2D& operator-=(const float num);
	JGAngle2D& operator*=(const float num);

	JGAngle2D& operator=(JGAngle2D& angle);
	JGAngle2D operator+(JGAngle2D& angle);
	JGAngle2D operator-(JGAngle2D& angle);
	JGAngle2D& operator=(const float num);
	JGAngle2D operator+(const float num);
	JGAngle2D operator-(const float num);


	bool operator==(JGAngle2D& angle);
	bool operator!=(JGAngle2D& angle);
	bool operator<(JGAngle2D& angle);
	bool operator<=(JGAngle2D& angle);
	bool operator>(JGAngle2D& angle);
	bool operator>=(JGAngle2D& angle);
	bool operator==(const float num);
	bool operator!=(const float num);
	bool operator<(const float num);
	bool operator<=(const float num);
	bool operator>(const float num);
	bool operator>=(const float num);

};

JGAngle2D operator+(const float num, JGAngle2D& angle);
JGAngle2D operator-(const float num, JGAngle2D& angle);


JGAngle2D operator*(JGAngle2D& angle, const float num);
JGAngle2D operator*(const float scalar, JGAngle2D& angle);

bool operator==(const float num, JGAngle2D& angle);
bool operator!=(const float num, JGAngle2D& angle);
bool operator<(const float num, JGAngle2D& angle);
bool operator<=(const float num, JGAngle2D& angle);
bool operator>(const float num, JGAngle2D& angle);
bool operator>=(const float num, JGAngle2D& angle);