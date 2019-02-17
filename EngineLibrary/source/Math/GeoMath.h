#pragma once
#include"CommonMath.h"

namespace JGLibrary
{
	class JVector2;
	class JVector3;
	class JPoint2;
	class JPoint3;
	class JNormal3;
	class JRay;

	class JVector2
	{
	public:
		xmfloat2 mData = { 0.0f,0.0f };
	public:
		JVector2() = default;
		JVector2(float x, float y) {
			mData.x = x; mData.y = y;
		}
		explicit JVector2(const JPoint2& p);
		explicit JVector2(const JPoint3& p);
		explicit JVector2(const JVector3& v);
		explicit JVector2(DirectX::FXMVECTOR sim) {
			SetSIMD(sim);
		}
		JVector2(const xmfloat2& v) : mData(v) {}
	public:
		JVector2& operator=(const JVector2& v) {
			mData = v.mData;
			return *this;
		}
		JVector2  operator+(const JVector2& v) const {
			return JVector2(mData.x + v.mData.x, mData.y + v.mData.y);
		}
		JVector2  operator-(const JVector2& v) const {
			return JVector2(mData.x - v.mData.x, mData.y - v.mData.y);
		}
		JVector2& operator+=(const JVector2& v) {
			mData.x += v.mData.x;
			mData.y += v.mData.y;
			return *this;
		}
		JVector2& operator-=(const JVector2& v) {
			mData.x -= v.mData.x;
			mData.y -= v.mData.y;
			return *this;
		}
	public:
		bool      operator==(const JVector2& v) const {
			return DirectX::XMVector2Equal(GetSIMD(), v.GetSIMD());
		}
		bool      operator!=(const JVector2& v) const {
			return !DirectX::XMVector2Equal(GetSIMD(), v.GetSIMD());
		}
	public:
		JVector2 operator*(float k) const {
			return JVector2(mData.x * k, mData.y * k);
		}
		JVector2 operator/(float k) const {
			float inv = 1.0f / k;
			return JVector2(mData.x * inv, mData.y * inv);
		}
		JVector2& operator*=(float k) {
			mData.x *= k;
			mData.y *= k;
			return *this;
		}
		JVector2& operator/=(float k) {
			float inv = 1.0f / k;
			mData.x *= inv;
			mData.y *= inv;
			return *this;
		}
	public:
		JVector2 operator-() {
			return JVector2(-mData.x, -mData.y);
		}

		float& operator[](jg_uint32 idx) {
			assert(idx == 0 || idx == 1);
			if (idx == 0)
				return mData.x;
			if (idx == 1)
				return mData.y;
			return JGLIB_DUMMY;
		}
		float  operator[](jg_uint32 idx) const {
			assert(idx == 0 || idx == 1);
			if (idx == 0)
				return mData.x;
			if (idx == 1)
				return mData.y;
			return JGLIB_DUMMY;
		}
	public:
		std::string ToString() const {
			std::string s_x = std::to_string(mData.x);
			std::string s_y = std::to_string(mData.y);
			std::string result = "[ " + s_x + ", " + s_y + "]";
			return result;
		}
		float LengthSqrd()     const {
			return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(GetSIMD()));
		}
		float Length()         const {
			return DirectX::XMVectorGetX(DirectX::XMVector2Length(GetSIMD()));
		}
		float Dot(const JVector2& v) const {
			return DirectX::XMVectorGetX(DirectX::XMVector2Dot(GetSIMD(), v.GetSIMD()));
		}
		float AbsDot(const JVector2& v) const {
			return abs(Dot(v));
		}
		void  Normalize() {
			SetSIMD(DirectX::XMVector2Normalize(GetSIMD()));
		}
	public:
		SimVector GetSIMD() const {
			return DirectX::XMLoadFloat2(&mData);
		}
		void SetSIMD(FSimVector v) {
			DirectX::XMStoreFloat2(&mData, v);
		}
	};
	class JVector3
	{
	public:
		xmfloat3 mData = { 0.0f,0.0f ,0.0f};
	public:
		JVector3() = default;
		JVector3(float x, float y, float z) {
			mData.x = x;
			mData.y = y;
			mData.z = z;
		}
		explicit JVector3(const JPoint3& p);
		explicit JVector3(const JNormal3& n);
		explicit JVector3(DirectX::FXMVECTOR sim)
		{
			SetSIMD(sim);
		}
		JVector3(const xmfloat3& v) : mData(v) {}
	public:
		JVector3& operator=(const JVector3& v) {
			mData = v.mData;
			return *this;
		}
		JVector3  operator+(const JVector3& v) const {
			return JVector3(mData.x + v.mData.x, mData.y + v.mData.y, mData.z + v.mData.z);
		}
		JVector3  operator-(const JVector3& v) const {
			return JVector3(mData.x - v.mData.x, mData.y - v.mData.y, mData.z - v.mData.z);
		}
		JVector3& operator+=(const JVector3& v) {
			mData.x += v.mData.x;
			mData.y += v.mData.y;
			mData.z += v.mData.z;
			return *this;
		}
		JVector3& operator-=(const JVector3& v) {
			mData.x -= v.mData.x;
			mData.y -= v.mData.y;
			mData.z -= v.mData.z;
			return *this;
		}
	public:
		bool      operator==(const JVector3& v) const {
			return DirectX::XMVector3Equal(GetSIMD(), v.GetSIMD());
		}
		bool      operator!=(const JVector3& v) const {
			return !DirectX::XMVector3Equal(GetSIMD(), v.GetSIMD());
		}
	public:
		JVector3 operator*(float k) const {
			return JVector3(mData.x * k, mData.y * k, mData.z * k);
		}
		JVector3 operator/(float k) const {
			float inv = 1.0f / k;
			return JVector3(mData.x * inv, mData.y * inv, mData.z * inv);
		}

		JVector3& operator*=(float k) {
			mData.x *= k;
			mData.y *= k;
			mData.z *= k;

			return *this;
		}
		JVector3& operator/=(float k) {
			float inv = 1.0f / k;
			mData.x *= inv;
			mData.y *= inv;
			mData.z *= inv;
			return *this;
		}
	public:
		JVector3 operator-() const {
			return JVector3(-mData.x, -mData.y, -mData.z);
		}
		float& operator[](jg_uint32 idx) {
			assert(idx == 0 || idx == 1 || idx == 2);
			if (idx == 0)
				return mData.x;
			if (idx == 1)
				return mData.y;
			if (idx == 2)
				return mData.z;
			return JGLIB_DUMMY;
		}
		float operator[](jg_uint32 idx) const {
			assert(idx == 0 || idx == 1 || idx == 2);
			if (idx == 0)
				return mData.x;
			if (idx == 1)
				return mData.y;
			if (idx == 2)
				return mData.z;
			return JGLIB_DUMMY;
		}
	public:
		std::string ToString() const {
			std::string s_x = std::to_string(mData.x);
			std::string s_y = std::to_string(mData.y);
			std::string s_z = std::to_string(mData.z);
			std::string result = "[ " + s_x + ", " + s_y + ", " + s_z + "]";
			return result;
		}
		float LengthSqrd() const {
			return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(GetSIMD()));
		}
		float Length() const{
			return DirectX::XMVectorGetX(DirectX::XMVector3Length(GetSIMD()));
		}
		float Dot(const JVector3& v) const{
			return DirectX::XMVectorGetX(DirectX::XMVector3Dot(GetSIMD(), v.GetSIMD()));
		}
		float AbsDot(const JVector3& v) const {
			return abs(Dot(v));
		}
		void  Normalize() {
			SetSIMD(DirectX::XMVector3Normalize(GetSIMD()));
		}
		JVector3  Cross(const JVector3& v) const {
			return JVector3(DirectX::XMVector3Cross(GetSIMD(), v.GetSIMD()));
		}
		void CoordinateSystem(JVector3* v1, JVector3* v2) {
			if (fabsf(mData.x) > fabsf(mData.y))
			{
				float invLen = 1.0f / sqrtf(mData.x * mData.x + mData.z * mData.z);
				v1->mData = { -mData.z * invLen, 0.0f, mData.x * invLen };
			}
			else
			{
				float invLen = 1.0f / sqrtf(mData.y * mData.y + mData.z * mData.z);
				v1->mData = { 0.0f, mData.z * invLen, -mData.y * invLen };
			}
			*v2 = Cross(*v2);
		}
	public:
		SimVector GetSIMD() const {
			return DirectX::XMLoadFloat3(&mData);
		}
		void SetSIMD(FSimVector  v) {
			DirectX::XMStoreFloat3(&mData, v);
		}
	};


	class JPoint2
	{
	public:
		xmfloat2 mData = { 0.0f,0.0f };
	public:
		JPoint2() = default;
		JPoint2(float x, float y) {
			mData.x = x;
			mData.y = y;
		}
		explicit JPoint2(const JPoint3& p);
		explicit JPoint2(const JVector2& v) : mData(v.mData) {}
		explicit JPoint2(DirectX::FXMVECTOR sim)
		{
			SetSIMD(sim);
		}
		JPoint2(const xmfloat2& p) : mData(p) {}
	public:
		JPoint2&  operator=(const JPoint2& p) {
			mData.x = p.mData.x;
			mData.y = p.mData.y;
			return *this;
		}
		JPoint2   operator+(const JVector2& v)  const {
			return JPoint2(mData.x + v.mData.x, mData.y + v.mData.y);
		}
		JPoint2   operator+(const JPoint2& p)   const {
			return JPoint2(mData.x + p.mData.x, mData.y + p.mData.y);
		}
		JPoint2&  operator+=(const JVector2& v) {
			mData.x += v.mData.x;
			mData.y += v.mData.y;

			return *this;
		}
		JPoint2&  operator+=(const JPoint2& p) {
			mData.x += p.mData.x;
			mData.y += p.mData.y;

			return *this;
		}
	public:
		JVector2  operator-(const JPoint2& p)   const {
			return JVector2(mData.x - p.mData.x, mData.y - p.mData.y);
		}
		JPoint2   operator-(const JVector2& v)  const {
			return JPoint2(mData.x - v.mData.x, mData.y - v.mData.y);
		}
		JPoint2&  operator-=(const JVector2& v) {
			mData.x -= v.mData.x;
			mData.y -= v.mData.y;
			return *this;
		}
	public:
		JPoint2  operator*(float k) const {
			return JPoint2(mData.x * k, mData.y * k);
		}
		JPoint2& operator*=(float k) {
			mData.x *= k;
			mData.y *= k;
			return *this;
		}
		JPoint2  operator/(float k) const {
			float inv = 1.0f / k;
			return JPoint2(mData.x * inv, mData.y * inv);
		}
		JPoint2& operator/=(float k) {
			float inv = 1.0f / k;
			mData.x /= inv;
			mData.y /= inv;
			return *this;
		}
		JPoint2  operator-() const {
			return JPoint2(-mData.x, -mData.y);
		}
	public:
		bool operator==(const JPoint2& p) const {
			return DirectX::XMVector2Equal(GetSIMD(), p.GetSIMD());
		}
		bool operator!=(const JPoint2& p) const {
			return !DirectX::XMVector2Equal(GetSIMD(), p.GetSIMD());
		}
	public:
		float& operator[](jg_uint32 idx) {
			assert(idx == 0 || idx == 1);
			if (idx == 0)
				return mData.x;
			if (idx == 1)
				return mData.y;
			return JGLIB_DUMMY;
		}
		float operator[](jg_uint32 idx) const {
			assert(idx == 0 || idx == 1);
			if (idx == 0)
				return mData.x;
			if (idx == 1)
				return mData.y;
			return JGLIB_DUMMY;
		}
		float DistanceSqrd(const JPoint2& p) const
		{
			return ((*this) - p).LengthSqrd();
		}
		float Distance(const JPoint2& p) const
		{
			return ((*this) - p).Length();
		}
		std::string ToString() const {
			std::string s_x = std::to_string(mData.x);
			std::string s_y = std::to_string(mData.y);
			std::string result = "[ " + s_x + ", " + s_y + "]";
			return result;
		}
	public:
		SimVector GetSIMD() const{
			return XMLoadFloat2(&mData);
		}
		void SetSIMD(FSimVector  v){
			XMStoreFloat2(&mData, v);
		}
	};

	class JPoint3
	{
	public:
		xmfloat3 mData = { 0.0f,0.0f,0.0f };
	public:
	    JPoint3() = default;
		JPoint3(float x, float y, float z) {
			mData.x = x; mData.y = y; mData.z = z;
		}
		explicit JPoint3(const JVector3& v) : mData(v.mData) {}
		explicit JPoint3(DirectX::FXMVECTOR sim)
		{
			SetSIMD(sim);
		}
		JPoint3(const xmfloat3& p) : mData(p) {}
	public:
		JPoint3&  operator=(const JPoint3& p) {
			mData = p.mData;
			return *this;
		}
		JPoint3   operator+(const JVector3& v)  const {
			return JPoint3(mData.x + v.mData.x, mData.y + v.mData.y, mData.z + v.mData.z);
		}
		JPoint3   operator+(const JPoint3& p)   const {
			return JPoint3(mData.x + p.mData.x, mData.y + p.mData.y, mData.z + p.mData.z);
		}
		JPoint3&  operator+=(const JVector3& v) {
			mData.x += v.mData.x;
			mData.y += v.mData.y;
			mData.z += v.mData.z;
			return *this;
		}
		JPoint3&  operator+=(const JPoint3& p) {
			mData.x += p.mData.x;
			mData.y += p.mData.y;
			mData.z += p.mData.z;
			return *this;
		}

	public:
		JVector3  operator-(const JPoint3& p)   const {
			return JVector3(mData.x - p.mData.x, mData.y - p.mData.y, mData.z - p.mData.z);
		}
		JPoint3   operator-(const JVector3& v)  const {
			return JPoint3(mData.x - v.mData.x, mData.y - v.mData.y, mData.z - v.mData.z);
		}
		JPoint3&  operator-=(const JVector3& v) {
			mData.x -= v.mData.x;
			mData.y -= v.mData.y;
			mData.z -= v.mData.z;
			return *this;
		}
	public:
		JPoint3  operator*(float k) const {
			return JPoint3(mData.x * k, mData.y *k, mData.z * k);
		}
		JPoint3& operator*=(float k) {
			mData.x *= k;
			mData.y *= k;
			mData.z *= k;
			return *this;
		}
		JPoint3  operator/(float k) const {
			float inv = 1.0f / k;
			return JPoint3(mData.x * inv, mData.y * inv, mData.z * inv);
		}
		JPoint3& operator/=(float k) {
			float inv = 1.0f / k;
			mData.x *= inv;
			mData.y *= inv;
			mData.z *= inv;
			return *this;
		}
		JPoint3  operator-() const {
			return JPoint3(-mData.x, -mData.y, -mData.z);
		}
	public:
		bool operator==(const JPoint3& p) const {
			return DirectX::XMVector3Equal(GetSIMD(), p.GetSIMD());
		}
		bool operator!=(const JPoint3& p) const {
			return !DirectX::XMVector3Equal(GetSIMD(), p.GetSIMD());
		}
	public:
		float& operator[](jg_uint32 idx) {
			assert(idx == 0 || idx == 1 || idx == 2);
			if (idx == 0)
				return mData.x;
			if (idx == 1)
				return mData.y;
			if (idx == 2)
				return mData.z;
			return JGLIB_DUMMY;
		}
		float operator[](jg_uint32 idx) const {
			assert(idx == 0 || idx == 1 || idx == 2);
			if (idx == 0)
				return mData.x;
			if (idx == 1)
				return mData.y;
			if (idx == 2)
				return mData.z;
			return JGLIB_DUMMY;
		}
		float DistanceSqrd(const JPoint3& p) const
		{
			return ((*this) - p).LengthSqrd();
		}
		float Distance(const JPoint3& p) const
		{
			return ((*this) - p).Length();
		}
		std::string ToString() const {
			std::string s_x = std::to_string(mData.x);
			std::string s_y = std::to_string(mData.y);
			std::string s_z = std::to_string(mData.z);
			std::string result = "[ " + s_x + ", " + s_y + ", " + s_z + "]";
			return result;
		}
	public:
		SimVector GetSIMD() const {
			return XMLoadFloat3(&mData);
		}
		void SetSIMD(FSimVector  v) {
			XMStoreFloat3(&mData, v);
		}
	};



	class JNormal3
	{
	public:
		xmfloat3 mData = { 0.0f,0.0f,0.0 };
	public:
		JNormal3() = default;
		JNormal3(float x, float y, float z) {
			mData.x = x; mData.y = y; mData.z = z;
		}
		explicit JNormal3(const JVector3& v) : mData(v.mData) {}
		JNormal3(const xmfloat3& n) : mData(n) {}
		explicit JNormal3(DirectX::FXMVECTOR sim)
		{
			SetSIMD(sim);
		}
	public:
		JNormal3& operator=(const JNormal3& n) {
			mData = n.mData;
			return *this;
		}
		//
		JNormal3  operator+(const JNormal3& n) const {
			return JNormal3(mData.x + n.mData.x, mData.y + n.mData.y, mData.z + n.mData.z);
		}
		JNormal3& operator+=(const JNormal3& n) {
			mData.x += n.mData.x;
			mData.y += n.mData.y;
			mData.z += n.mData.z;
			return *this;
		}
		JNormal3  operator-(const JNormal3& n) const {
			return JNormal3(mData.x - n.mData.x, mData.y - n.mData.y, mData.z - n.mData.z);
		}
		JNormal3& operator-=(const JNormal3& n) {
			mData.x -= n.mData.x;
			mData.y -= n.mData.y;
			mData.z -= n.mData.z;
			return *this;
		}
	public:
		JNormal3  operator*(float k) const {
			return JNormal3(mData.x * k, mData.y * k, mData.z * k);
		}
		JNormal3& operator*=(float k) {
			mData.x *= k;
			mData.y *= k;
			mData.z *= k;
			return *this;
		}

		JNormal3  operator/(float k) const {
			float inv = 1.0f / k;
			return JNormal3(mData.x * inv, mData.y * inv, mData.z * inv);
		}
		JNormal3& operator/=(float k) {
			float inv = 1.0f / k;
			mData.x *= inv;
			mData.y *= inv;
			mData.z *= inv;
			return *this;
		}
	public:
		bool operator==(const JNormal3& n) const {
			return DirectX::XMVector3Equal(GetSIMD(), n.GetSIMD());
		}
		bool operator!=(const JNormal3& n) const {
			return !DirectX::XMVector3Equal(GetSIMD(), n.GetSIMD());
		}
	public:
		JNormal3 operator-() const {
			return JNormal3(-mData.x, -mData.y, -mData.z);
		}
		float&   operator[](jg_uint32 idx){
			assert(idx == 0 || idx == 1 || idx == 2);
			if (idx == 0)
				return mData.x;
			if (idx == 1)
				return mData.y;
			if (idx == 2)
				return mData.z;
			return JGLIB_DUMMY;
		}
		float    operator[](jg_uint32 idx) const {
			assert(idx == 0 || idx == 1 || idx == 2);
			if (idx == 0)
				return mData.x;
			if (idx == 1)
				return mData.y;
			if (idx == 2)
				return mData.z;
			return JGLIB_DUMMY;
		} 
	public:
		std::string ToString() const {
			std::string s_x = std::to_string(mData.x);
			std::string s_y = std::to_string(mData.y);
			std::string s_z = std::to_string(mData.z);
			std::string result = "[ " + s_x + ", " + s_y + ", " + s_z + "]";
			return result;
		}
		void Normalize() {
			SetSIMD(DirectX::XMVector3Normalize(GetSIMD()));
		}
		float LengthSqrd() const {
			return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(GetSIMD()));
		}
		float Length() const {
			return DirectX::XMVectorGetX(DirectX::XMVector3Length(GetSIMD()));
		}
	public:
		SimVector GetSIMD() const {
			return DirectX::XMLoadFloat3(&mData);
		}
		void SetSIMD(FSimVector v) {
			DirectX::XMStoreFloat3(&mData, v);
		}
	};

	class JRay
	{
	public:
		JPoint3  o; // 원점
		JVector3 d; // 방향
		mutable float tmax;
		float time;  // 시간
		int   depth; // 몇번 반사
	public:
		JRay() : tmax(INFINITY), time(0.0f), depth(0) {}
		JRay(const JPoint3 oo,
			const JVector3& dd,
			float ttmax = INFINITY,
			float ttime = 0.0f,
			int ddepth = 0) :
			o(oo), d(dd), tmax(ttmax), time(ttime), depth(ddepth) {}
		JPoint3 operator()(float t)const {
			return o + d * time; // ( O + V * t) Ray 공식
		}
		virtual std::string ToString() const {
			std::string maxTimeStr = "Max Time     : " + std::to_string(tmax) + "\n";
			std::string oStr       = "O(origin)    : " + o.ToString() + "\n";
			std::string dStr       = "D(direction) : " + d.ToString() + "\n";
			std::string tStr       = "Time         : " + std::to_string(time) + "\n";
			std::string depthStr   = "Depth(reflection count) : " + std::to_string(depth) + "\n";
			return maxTimeStr + oStr + dStr + tStr + depthStr;
		}
	};

	class JRayDifferential : public JRay
	{
	public:
		bool hasDifferentials = false;
		JPoint3  rxO, ryO;
		JVector3 rxD, ryD;
	public:
		JRayDifferential() = default;
		JRayDifferential(const JPoint3& o, const JVector3& d, float tmax, float time, int depth)
			: JRay(o, d, tmax, time, depth) {}
		void ScaleDifferentials(float s) {
			rxO = o + (rxO - o) * s;
			ryO = o + (ryO - o) * s;
			rxD = d + (rxD - d) * s;
			ryD = d + (ryD - d) * s;
		}
		virtual std::string ToString() const override {
			std::string rayStr = JRay::ToString();
			std::string s = "Has Differentials ? : ";
			(hasDifferentials) ? s += "true" : s += "false";
			
			std::string rxStr = "RXO(ray_x_origin)    : " + rxO.ToString() + "\n" +
				                "RXD(ray_x_direction) : " + rxD.ToString() + "\n";
			std::string ryStr = "RYO(ray_y_origin)    : " + ryO.ToString() + "\n" +
				                "RYD(ray_y_direction) : " + ryD.ToString() + "\n";
			return rayStr + s + rxStr + ryStr;
		}
	};


	class JBound2 // 700 줄 대 다시 이어서하기..
	{
	public:
		JPoint2 pMin, pMax;
	public:
		JBound2() {
			pMin = JPoint2(-INFINITY, -INFINITY);
			pMax = JPoint2(INFINITY, INFINITY);
		}
		JBound2(const JPoint2& p1, const JPoint2& p2) {
			pMin = JPoint2(
				std::fminf(p1.mData.x, p2.mData.x),
				std::fminf(p1.mData.y, p2.mData.y));
			pMax = JPoint2(
				std::fmaxf(p1.mData.x, p2.mData.x),
				std::fmaxf(p1.mData.y, p2.mData.y));
		}
		explicit JBound2(const JPoint2& p) : pMin(p), pMax(p) {}
		explicit operator JBound2() const {
			return JBound2(pMin, pMax);
		}
	public:
		JVector2 Diagonal() const { return pMax - pMin; }
		float    Area() const {
			JVector2 v = pMax - pMin;
			return (v.mData.x * v.mData.y);
		}
		int MaximumExtent() const {
			JVector2 diag = Diagonal();
			if (diag.mData.x > diag.mData.y)
				return 0; // x
			else
				return 1; // y
		}
		const JPoint2& operator[](int idx) const {
			assert(idx == 0 || idx == 1);
			return (idx == 0) ? pMin : pMax;
		}
		JPoint2& operator[](int idx) {
			assert(idx == 0 || idx == 1);
			return (idx == 0) ? pMin : pMax;
		}
		bool operator==(const JBound2& b) const {
			return pMin == b.pMin && pMax == b.pMax;
		}
		bool operator!=(const JBound2& b) const {
			return pMin != b.pMin || pMax != b.pMax;
		}

		JPoint2 Lerp(const JPoint2& t) const {
			return JPoint2(JGLibrary::Lerp(pMin.mData.x, pMax.mData.x, t.mData.x),
				JGLibrary::Lerp(pMin.mData.y, pMax.mData.y, t.mData.y));
		}
		JVector2 Offset(const JPoint2& p) const {
			JVector2 o = p - pMin;
			if (pMax.mData.x > pMin.mData.x) o.mData.x /= pMax.mData.x - pMin.mData.x;
			if (pMax.mData.y > pMin.mData.y) o.mData.y /= pMax.mData.y - pMin.mData.y;
			return o;
		}
		bool Inside(const JPoint2& p) const
		{
			bool p_greater_min = DirectX::XMVector3GreaterOrEqual(p.GetSIMD(), pMin.GetSIMD());
			bool p_less_max = DirectX::XMVector3LessOrEqual(p.GetSIMD(), pMax.GetSIMD());

			return p_greater_min && p_less_max;
		}
		void BoundingSphere(JPoint2* c, float* rad) const {
			*c = (pMin + pMax) / 2;
			*rad = Inside(*c) ? c->Distance(pMax) : 0;
		}
	public:
		std::string ToString() const {
			std::string minStr = "Min : " + pMin.ToString() + "\n";
			std::string maxStr = "Max : " + pMax.ToString() + "\n";
			return minStr + maxStr;
		}
	};
	class JBound3
	{
	public:
		JPoint3 pMin, pMax;
	public:
		JBound3() {
			pMin = JPoint3(-INFINITY, -INFINITY, -INFINITY);
			pMax = JPoint3(INFINITY, INFINITY, INFINITY);
		}
		explicit JBound3(const JPoint3& p) : pMin(p), pMax(p) {}
		JBound3(const JPoint3& p1, const JPoint3& p2) :
			pMin(std::fminf(p1.mData.x, p2.mData.x),
				std::fminf(p1.mData.y, p2.mData.y),
				std::fminf(p1.mData.z, p1.mData.z)),
			pMax(std::fmaxf(p1.mData.x, p2.mData.x),
				std::fmaxf(p1.mData.y, p2.mData.y),
				std::fmaxf(p1.mData.z, p2.mData.z)) {}
		explicit operator JBound3() const {
			return JBound3(pMin, pMax);
		}
	public:
		const JPoint3& operator[](jg_uint32 idx) const {
			assert(idx == 0 || idx == 1);
			return (idx == 0) ? pMin : pMax;
		}
		JPoint3& operator[](jg_uint32 idx) {
			assert(idx == 0 || idx == 1);
			return (idx == 0) ? pMin : pMax;
		}
	public:
		bool operator==(const JBound3& b) const {
			return (pMin == b.pMin && pMax == b.pMax);
		}
		bool operator!=(const JBound3& b) const {
			return (pMin != b.pMin || pMax != b.pMax);
		}
	public:
		JPoint3 Corner(int corner) const {
			assert(corner >= 0 && corner < 8);
			return JPoint3((*this)[(corner & 1)].mData.x,
				(*this)[(corner & 2) ? 1 : 0].mData.y,
				(*this)[(corner & 4) ? 1 : 0].mData.z);
		}
		JVector3 Diagonal() const { return pMax - pMin; }
		float SurfaceArea() const {
			JVector3 d = Diagonal();
			return 2 * (d.mData.x * d.mData.y + d.mData.x * d.mData.z + d.mData.y * d.mData.z);
		}
		float Volume() const {
			JVector3 d = Diagonal();
			return d.mData.x * d.mData.y * d.mData.z;
		}
		int MaximumExtent() const {
			JVector3 d = Diagonal();
			if (d.mData.x > d.mData.y && d.mData.x > d.mData.z)
				return 0;
			else if (d.mData.y > d.mData.z)
				return 1;
			else
				return 2;
		}
		JPoint3 Lerp(const JPoint3& t) const {
			return JPoint3(
				JGLibrary::Lerp(pMin.mData.x, pMax.mData.x, t.mData.x),
				JGLibrary::Lerp(pMin.mData.y, pMax.mData.y, t.mData.y),
				JGLibrary::Lerp(pMin.mData.z, pMax.mData.z, t.mData.z));
		}
		JVector3 Offset(const JPoint3& p) const {
			JVector3 o = p - pMin;
			if (pMax.mData.x > pMin.mData.x) o.mData.x /= pMax.mData.x - pMin.mData.x;
			if (pMax.mData.y > pMin.mData.y) o.mData.y /= pMax.mData.y - pMin.mData.y;
			if (pMax.mData.z > pMin.mData.z) o.mData.z /= pMax.mData.z - pMin.mData.z;
			return o;
		}
		bool Inside(const JPoint3& p) const
		{
			bool p_greater_min = DirectX::XMVector3GreaterOrEqual(p.GetSIMD(), pMin.GetSIMD());
			bool p_less_max = DirectX::XMVector3LessOrEqual(p.GetSIMD(), pMax.GetSIMD());

			return p_greater_min && p_less_max;
		}
		void BoundingSphere(JPoint3 *center, float *radius) const {
			*center = (pMin + pMax) / 2;
			*radius = Inside(*center) ? center->Distance(pMax) : 0;
		}
		bool IntersectP(const JRay& ray, float* hitt0 = nullptr, float* hitt1 = nullptr) const {
			float t0 = 0, t1 = ray.tmax;
			for (int i = 0; i < 3; ++i) {
			
				float invRayDir = 1 / ray.d[i];
				float tNear = (pMin[i] - ray.o[i]) * invRayDir;
				float tFar = (pMax[i] - ray.o[i]) * invRayDir;

	
				if (tNear > tFar) std::swap(tNear, tFar);

				
				tFar *= 1 + 2 * Gamma(3);
				t0 = tNear > t0 ? tNear : t0;
				t1 = tFar < t1 ? tFar : t1;
				if (t0 > t1) return false;
			}
			if (hitt0) *hitt0 = t0;
			if (hitt1) *hitt1 = t1;
			return true;
		}
		bool IntersectP(const JRay& ray, const JVector3& invDir, const int dirIsNeg[3]) const {
			const JBound3 &bounds = *this;

			float tMin = (bounds[dirIsNeg[0]].mData.x - ray.o.mData.x) * invDir.mData.x;
			float tMax = (bounds[1 - dirIsNeg[0]].mData.x - ray.o.mData.x) * invDir.mData.x;
			float tyMin = (bounds[dirIsNeg[1]].mData.y - ray.o.mData.y) * invDir.mData.y;
			float tyMax = (bounds[1 - dirIsNeg[1]].mData.y - ray.o.mData.y) * invDir.mData.y;


			tMax *= 1 + 2 * Gamma(3);
			tyMax *= 1 + 2 * Gamma(3);
			if (tMin > tyMax || tyMin > tMax) return false;
			if (tyMin > tMin) tMin = tyMin;
			if (tyMax < tMax) tMax = tyMax;

			float tzMin = (bounds[dirIsNeg[2]].mData.z - ray.o.mData.z) * invDir.mData.z;
			float tzMax = (bounds[1 - dirIsNeg[2]].mData.z - ray.o.mData.z) * invDir.mData.z;

			tzMax *= 1 + 2 * Gamma(3);
			if (tMin > tzMax || tzMin > tMax) return false;
			if (tzMin > tMin) tMin = tzMin;
			if (tzMax < tMax) tMax = tzMax;
			return (tMin < ray.tmax) && (tMax > 0);
		}
	};
	// JVector2 inline function

	inline JVector2 operator*(float k, const JVector2& v){
		return v * k;
	}
	inline float Dot(const JVector2& v1, const JVector2& v2){
		return DirectX::XMVectorGetX(DirectX::XMVector2Dot(v1.GetSIMD(), v2.GetSIMD()));
	}

	inline float AbsDot(const JVector2& v1, const JVector2& v2){
		return std::abs(Dot(v1, v2));
	}
	inline JVector2 Normalize(const JVector2& v){
		return JVector2(DirectX::XMVector2Normalize(v.GetSIMD())); 
	}
	inline JVector2 Abs(const JVector2& v){
		return JVector2(std::abs(v[0]), std::abs(v[1]));
	}
	// JVector3 inline function
	inline JVector3 operator*(float k, const JVector3& v){
		return v * k;
	}
	inline JVector3 Abs(const JVector3& v){
		return JVector3(std::abs(v.mData.x), std::abs(v.mData.y), std::abs(v.mData.z));
	}
	inline float Dot(const JVector3& v1, const JVector3& v2){
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(v1.GetSIMD(), v2.GetSIMD()));
	}
	inline float AbsDot(const JVector3& v1, const JVector3& v2){
		return std::abs(Dot(v1, v2));
	}
	inline JVector3 Cross(const JVector3& v1, const JVector3& v2){
		return JVector3(DirectX::XMVector3Cross(v1.GetSIMD(), v2.GetSIMD()));
	}
	inline JVector3 Cross(const JVector3& v, const JNormal3& n){
		return JVector3(DirectX::XMVector3Cross(v.GetSIMD(), n.GetSIMD()));
	}
	inline JVector3 Cross(const JNormal3& n, const JVector3& v){
		return JVector3(DirectX::XMVector3Cross(n.GetSIMD(), v.GetSIMD()));
	}
	inline JVector3 Normalize(const JVector3& v){
		return JVector3(DirectX::XMVector3Normalize(v.GetSIMD()));
	}
	inline float MinComponent(const JVector3& v){
		return std::fminf(v.mData.x, std::fminf(v.mData.y, v.mData.z));
	}
	inline float MaxComponent(const JVector3& v){
		return std::fmaxf(v.mData.x, std::fmaxf(v.mData.y, v.mData.z));
	}
	inline JVector3 Min(const JVector3& v1, const JVector3& v2){
		return JVector3(std::fminf(v1[0], v2[0]), std::fminf(v1[1], v2[1]), std::fminf(v1[2], v2[2]));
	}
	inline JVector3 Max(const JVector3& v1, const JVector3& v2){
		return JVector3(std::fmaxf(v1[0], v2[0]), std::fmaxf(v1[1], v2[1]), std::fmaxf(v1[2], v2[2]));
	}
	inline JVector3 Permute(const JVector3& v, int x, int y, int z){
		return JVector3(v[x], v[y], v[z]);
	}
	inline JVector3 CoordinateSystem(const JVector3& v1, JVector3* v2, JVector3* v3){
		if (std::abs(v1.mData.x) > std::abs(v1.mData.y))
			*v2 = JVector3(-v1.mData.z, 0, v1.mData.x) / std::sqrt(v1.mData.x * v1.mData.x + v1.mData.z * v1.mData.z);
		else
			*v2 = JVector3(0, v1.mData.z, -v1.mData.y) / std::sqrt(v1.mData.y * v1.mData.y + v1.mData.z * v1.mData.z);
		*v3 = Cross(v1, *v2);
	}

	//// JPoint3 inline function
	inline float Distance(const JPoint3& p1, const JPoint3& p2){
		return (p1 - p2).Length();
	}
	inline float DistanceSqrd(const JPoint3& p1, const JPoint3& p2){
		return (p1 - p2).LengthSqrd();
	}
	inline JPoint3 operator*(float f, const JPoint3& p){
		return p * f;
	}
	inline JPoint3 Lerp(const JPoint3& p1, const JPoint3& p2, float t){
		return JPoint3(DirectX::XMVectorLerp(p1.GetSIMD(), p2.GetSIMD(), t));
	}
	inline JPoint3 Min(const JPoint3& p1, const JPoint3& p2){
		return JPoint3(
			std::fminf(p1.mData.x, p2.mData.x),
			std::fminf(p1.mData.y, p2.mData.y),
			std::fminf(p1.mData.z, p2.mData.z));
	}
	inline JPoint3 Max(const JPoint3& p1, const JPoint3& p2){
		return JPoint3(
			std::fmaxf(p1.mData.x, p2.mData.x),
		    std::fmaxf(p1.mData.y, p2.mData.y),
			std::fmaxf(p1.mData.y, p2.mData.z));
	}
	inline JPoint3 Floor(const JPoint3& p){
		return JPoint3(std::floor(p.mData.x), std::floor(p.mData.y), std::floor(p.mData.z));
	}
	inline JPoint3 Ceil(const JPoint3& p){
		return JPoint3(std::ceil(p.mData.x), std::ceil(p.mData.y), std::ceil(p.mData.z));
	}
	inline JPoint3 Abs(const JPoint3& p){
		return JPoint3(std::abs(p.mData.x), std::abs(p.mData.y), std::abs(p.mData.z));
	}


	//// JPoint2 inline function

	inline float Distance(const JPoint2& p1, const JPoint2& p2){
		return (p1 - p2).Length();
	}
	inline float DistanceSqrd(const JPoint2& p1, const JPoint2& p2){
		return (p1 - p2).LengthSqrd();
	}
	inline JPoint2 operator*(float k, const JPoint2& p){
		return p * k;
	}
	inline JPoint2 Floor(const JPoint2& p){
		return JPoint2(std::floor(p.mData.x), std::floor(p.mData.y));
	}
	inline JPoint2 Ceil(const JPoint2& p){
		return JPoint2(std::ceil(p.mData.x), std::floor(p.mData.y));
	}
	inline JPoint2 Lerp(const JPoint2& p1, const JPoint2& p2, float t){
		return JPoint2(DirectX::XMVectorLerp(p1.GetSIMD(), p2.GetSIMD(), t));
	}
	inline JPoint2 Min(const JPoint2& p1, const JPoint2& p2){
		return JPoint2(std::fminf(p1.mData.x, p2.mData.x), std::fminf(p1.mData.y, p2.mData.y));
	}
	inline JPoint2 Max(const JPoint2& p1, const JPoint2& p2){
		return JPoint2(std::fmaxf(p1.mData.x, p2.mData.x), std::fmaxf(p1.mData.y, p2.mData.y));
	}
	inline JPoint3 Permute(const JPoint3& p, int x, int y, int z){
		return JPoint3(p[x], p[y], p[z]);
	}




	//// Normal3 inline function
	inline JNormal3 operator*(float k, const JNormal3& n){
		return n * k;
	}
	inline JNormal3 Normalize(const JNormal3& n){
		return JNormal3(DirectX::XMVector3Normalize(n.GetSIMD()));
	}
	inline float Dot(const JNormal3& n, const JVector3& v){
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(n.GetSIMD(), v.GetSIMD()));
	}
	inline float Dot(const JVector3& v, const JNormal3& n){
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(v.GetSIMD(), n.GetSIMD()));
	}
	inline float Dot(const JNormal3& n1, const JNormal3& n2){
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(n1.GetSIMD(), n2.GetSIMD()));
	}
	inline float AbsDot(const JNormal3& n, const JVector3& v){
		return std::abs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(n.GetSIMD(), v.GetSIMD())));
	}
	inline float AbsDot(const JVector3& v, const JNormal3& n){
		return std::abs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(v.GetSIMD(), n.GetSIMD())));
	}
	inline float AbsDot(const JNormal3& n1, const JNormal3& n2){
		return std::abs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(n1.GetSIMD(), n2.GetSIMD())));
	}
	inline JNormal3 Facefoward(const JNormal3& n, const JVector3& v) {
		return (Dot(n, v) < 0.0f) ? -n : n;
	}
	inline JNormal3 Facefoward(const JNormal3& n1, const JNormal3& n2) {
		return (Dot(n1, n2) < 0.0f) ? -n1 : n1;
	}
	inline JVector3 Facefoward(const JVector3& v1, const JVector3& v2) {
		return (Dot(v1, v2) < 0.0f) ? -v1 : v1;
	}
	inline JVector3 Facefoward(const JVector3& v, const JNormal3& n) {
		return (Dot(v, v) < 0.0f) ? -v : v;
	}
	inline JNormal3 Abs(const JNormal3& n){
		return JNormal3(std::abs(n.mData.x), std::abs(n.mData.y), std::abs(n.mData.z));
	}



	// JBound2 
	inline  JBound2 Union(const JBound2& b, const JPoint2& p) {
		JBound2 ret;
		ret.pMin = Min(b.pMin, p);
		ret.pMax = Max(b.pMax, p);
		return ret;
	}
	inline JBound2 Union(const JBound2& b1, const JBound2& b2) {
		JBound2 ret;
		ret.pMin = Min(b1.pMin, b2.pMin);
		ret.pMax = Max(b1.pMax, b2.pMax);
		return ret;
	}
	inline bool Overlap(const JBound2& b1, const JBound2& b2) {
		bool greater = DirectX::XMVector3GreaterOrEqual(b1.pMax.GetSIMD(), b2.pMin.GetSIMD());
		bool less = DirectX::XMVector3LessOrEqual(b1.pMin.GetSIMD(), b2.pMax.GetSIMD());
		return greater && less;
	}
	inline bool Inside(const JPoint2& p, const JBound2& b) {

		bool p_greater_min = DirectX::XMVector3GreaterOrEqual(p.GetSIMD(), b.pMin.GetSIMD());
		bool p_less_max = DirectX::XMVector3LessOrEqual(p.GetSIMD(), b.pMax.GetSIMD());

		return p_greater_min && p_less_max;
	}
	inline bool InsideExclusive(const JPoint2& p, const JBound2& b) {
		bool p_greater_min = DirectX::XMVector3GreaterOrEqual(p.GetSIMD(), b.pMin.GetSIMD());
		bool p_less_max = DirectX::XMVector3Less(p.GetSIMD(), b.pMax.GetSIMD());

		return p_greater_min && p_less_max;
	}
	inline JBound2 Expand(const JBound2& b, float delta) {
		return JBound2(b.pMin - JVector2(delta, delta),
			b.pMax + JVector2(delta, delta));
	}
	inline float DistanceSqrd(const JPoint2& p, const JBound2& b) {
		float dx = std::max<float>({ 0.0f, b.pMin.mData.x - p.mData.x, p.mData.x - b.pMax.mData.x });
		float dy = std::max<float>({ 0.0f, b.pMin.mData.y - p.mData.y, p.mData.y - b.pMax.mData.y });
		return dx * dx + dy * dy;
	}
	inline float Distance(const JPoint2& p, const JBound2& b) {
		return std::sqrtf(DistanceSqrd(p, b));
	}

	// JBound3
	inline JBound3 Union(const JBound3& b, const JPoint3& p) {
		JBound3 ret;
		ret.pMin = Min(b.pMin, p);
		ret.pMax = Max(b.pMax, p);
		return ret;
	}
	inline JBound3 Union(const JBound3& b1, const JBound3& b2) {
		JBound3 ret;
		ret.pMin = Min(b1.pMin, b2.pMin);
		ret.pMax = Max(b1.pMax, b2.pMax);
		return ret;
	}
	inline JBound3 Intersect(const JBound3& b1, const JBound3& b2) {
		JBound3 ret;
		ret.pMin = Max(b1.pMin, b2.pMin);
		ret.pMax = Min(b1.pMax, b2.pMax);
		return ret;
	}
	inline bool Overlaps(const JBound3& b1, const JBound3& b2) {

		bool greater = DirectX::XMVector3GreaterOrEqual(b1.pMax.GetSIMD(), b2.pMin.GetSIMD());
		bool less    = DirectX::XMVector3LessOrEqual(b1.pMin.GetSIMD(), b2.pMax.GetSIMD());
		return greater && less;
	}
	inline bool Inside(const JPoint3& p, const JBound3& b) {

		bool p_greater_min = DirectX::XMVector3GreaterOrEqual(p.GetSIMD(), b.pMin.GetSIMD());
		bool p_less_max    = DirectX::XMVector3LessOrEqual(p.GetSIMD(), b.pMax.GetSIMD());

		return p_greater_min && p_less_max;
	}
	inline bool InsideExclusive(const JPoint3& p, const JBound3& b) {
		bool p_greater_min = DirectX::XMVector3GreaterOrEqual(p.GetSIMD(), b.pMin.GetSIMD());
		bool p_less_max    = DirectX::XMVector3Less(p.GetSIMD(), b.pMax.GetSIMD());

		return p_greater_min && p_less_max;
	}
	inline JBound3 Expand(const JBound3& b, float delta) {
		return JBound3(b.pMin - JVector3(delta, delta, delta),
			           b.pMax + JVector3(delta, delta, delta));
	}
	inline float DistanceSqrd(const JPoint3& p, const JBound3& b) {
		float dx = std::max<float>({ 0.0f, b.pMin.mData.x - p.mData.x, p.mData.x - b.pMax.mData.x });
		float dy = std::max<float>({ 0.0f, b.pMin.mData.y - p.mData.y, p.mData.y - b.pMax.mData.y });
		float dz = std::max<float>({ 0.0f, b.pMin.mData.z - p.mData.z, p.mData.z - b.pMax.mData.z });
		return dx * dx + dy * dy + dz * dz;
	}
	inline float Distance(const JPoint3& p, const JBound3& b) {
		return std::sqrtf(DistanceSqrd(p, b));
	}

	// 
	inline JPoint3 OffsetRayOrigin(const JPoint3& p, const JVector3& pError,
		const JNormal3& n, const JVector3& w) {
		float d = Dot(Abs(n), pError);
		JVector3 offset = d * JVector3(n);
		if (Dot(w, n) < 0) offset = -offset;
		JPoint3 po = p + offset;
	
		for (int i = 0; i < 3; ++i) {
			if (offset[i] > 0)
				po[i] = NextFloatUp(po[i]);
			else if (offset[i] < 0)
				po[i] = NextFloatDown(po[i]);
		}
		return po;

	}
	inline JVector3 SphericalDirection(float sinTheta, float cosTheta, float phi) {
		return JVector3(sinTheta * std::cos(phi), sinTheta * std::sin(phi),
			cosTheta);
	}
	inline JVector3 SphericalDirection(float sinTheta, float cosTheta, float phi,
		const JVector3 &x, const JVector3 &y,
		const JVector3 &z) {
		return sinTheta * std::cos(phi) * x + sinTheta * std::sin(phi) * y +
			cosTheta * z;
	}
	inline float SphericalTheta(const JVector3 &v) {
		return std::acos(Clamp(v.mData.z, -1, 1));
	}

	inline float SphericalPhi(const JVector3 &v) {
		float p = std::atan2(v.mData.y, v.mData.x);
		return (p < 0) ? (p + 2 * JG_PI) : p;
	}
}
