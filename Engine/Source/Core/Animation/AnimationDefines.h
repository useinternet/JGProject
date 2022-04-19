#pragma once


enum class EAnimationParameterType
{
	Unknown,
	Bool,
	Float,
	Int,
	Vector2,
	Vector3,
	Vector4
};
enum class EAnimationCondition
{
	Equal,
	EqualOver,
	EqualLess,
	Over,
	Less,
};