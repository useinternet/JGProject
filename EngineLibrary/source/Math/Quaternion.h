#pragma once
#include"CommonMath.h"


namespace JGLibrary
{
	class Transform;
	class Quaternion
	{
	public:
		xmfloat4 mData = { 0.0f,0.0f,0.0f,1.0f };
	public:
		Quaternion(float x, float y, float z, float w) :
			mData(xmfloat4(x, y, z, w)) {}
		Quaternion(const xmfloat4& q) : mData(q) {}
		Quaternion(const Transform& form);
		Quaternion(FSimVector v) {
			SetSIMD(v);
		}
	public:
		Quaternion  operator+ (const Quaternion& q) const {
			return Quaternion(
				mData.x + q.mData.x, mData.y + q.mData.y,
				mData.z + q.mData.z, mData.w + q.mData.w);
		}
		Quaternion& operator+=(const Quaternion& q) {
			mData.x += q.mData.x;
			mData.y += q.mData.y;
			mData.z += q.mData.z;
			mData.w += q.mData.w;
			return *this;
		}
		Quaternion  operator- (const Quaternion& q) const {
			return Quaternion(
				mData.x - q.mData.x, mData.y - q.mData.y,
				mData.z - q.mData.z, mData.w - q.mData.w);
		}
		Quaternion& operator-=(const Quaternion& q) {
			mData.x -= q.mData.x;
			mData.y -= q.mData.y;
			mData.z -= q.mData.z;
			mData.w -= q.mData.w;
			return *this;
		}
		Quaternion operator-() const {
			return Quaternion(-mData.x, -mData.y, -mData.z, -mData.w);
		}
		Quaternion operator*(float k) const {
			return Quaternion(mData.x  * k, mData.y * k, mData.z * k, mData.w * k);
		}
		Quaternion& operator*=(float k) {
			mData.x *= k;
			mData.y *= k;
			mData.z *= k;
			mData.w *= k;
			return *this;
		}
		Quaternion operator/(float k) const {
			float inv = 1.0f / k;
			return Quaternion(mData.x  * inv, mData.y * inv, mData.z * inv, mData.w * inv);
		}
		Quaternion operator/=(float k) {
			float inv = 1.0f / k;
			mData.x *= inv;
			mData.y *= inv;
			mData.z *= inv;
			mData.w *= inv;
			return *this;
		}
		Transform ToTransform() const;
		float Dot(const Quaternion& q) const {
			SimVector v = DirectX::XMQuaternionDot(GetSIMD(), q.GetSIMD());
			return DirectX::XMVectorGetX(v);
		}
		void Normalize() {
			SimVector v = DirectX::XMQuaternionNormalize(GetSIMD());
			SetSIMD(v);
		}
	public:
		SimVector GetSIMD() const {
			return DirectX::XMLoadFloat4(&mData);
		}
		void SetSIMD(FSimVector vec) {
			DirectX::XMStoreFloat4(&mData, vec);
		}
	};
	Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t) {
		SimVector v = DirectX::XMQuaternionSlerp(q1.GetSIMD(), q2.GetSIMD(), t);
		return Quaternion(v);
	}
	inline Quaternion operator*(float k, const Quaternion& q) {
		return q * k;
	}
	inline float Dot(const Quaternion& q1, const Quaternion& q2) {
		SimVector v = DirectX::XMQuaternionDot(q1.GetSIMD(), q2.GetSIMD());
		return DirectX::XMVectorGetX(v);
	}
	inline Quaternion Normalize(const Quaternion& q) {
		SimVector v = DirectX::XMQuaternionNormalize(q.GetSIMD());
		return Quaternion(v);
	}

}