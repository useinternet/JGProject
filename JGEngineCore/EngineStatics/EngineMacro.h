
// 유니코드 변환
#define TT(x) L##x

// EnumClass 를 int로 변환(배열 인덱스용)
#define EnumClassToInt(x) \
static_cast<int>(x##)     \


// 
#define JG_PI    (3.14159265358979323846)
inline float OneByPI()
{
	return (1.0f / (float)JG_PI);
}
inline float ToRadian(const float Degree)
{
	return (Degree * ((float)JG_PI / 180.0f));
}

inline float ToDegree(const float Radian)
{
	return (Radian * (180.0f / (float)JG_PI));
}
