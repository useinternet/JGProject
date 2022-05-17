#pragma once
#include "Math.h"
#include <DirectXMath.h>

namespace JG
{
	class JVector3;
	class JMatrix;
	class JQuaternion
	{
		friend JMatrix;
	public:
		float x, y, z, w;
	public:
		JQuaternion(const JVector3& v);
		JQuaternion(float pitch, float yaw, float roll);
		JQuaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) :
			x(x), y(y), z(z), w(w) {}
		JQuaternion(const JQuaternion& copy) = default;




		JQuaternion& operator=(const JQuaternion& q) 
		{
			x = q.x;  y = q.y;  z = q.z; w = q.w;
			return *this;
		}

		JQuaternion operator*(const JQuaternion& q) const
		{
			JQuaternion result;
			result.SetSIMD(DirectX::XMQuaternionMultiply(GetSIMD(), q.GetSIMD()));
			return result;
		}
		JQuaternion& operator*=(const JQuaternion& q)
		{
			SetSIMD(DirectX::XMQuaternionMultiply(GetSIMD(), q.GetSIMD()));
			return *this;
		}
		bool operator==(const JQuaternion& q) const
		{
			return DirectX::XMQuaternionEqual(GetSIMD(), q.GetSIMD());
		}
		bool operator!=(const JQuaternion& q) const
		{
			return !DirectX::XMQuaternionEqual(GetSIMD(), q.GetSIMD());
		}

		inline float& operator[](int idx) {
			switch (idx)
			{
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 3:
				return w;
			}
			assert(false && "Quaternion Index exceed..");
			return x;
		}
		inline float At(int idx) const {
			switch (idx)
			{
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 3:
				return w;
			}
			assert(false && "Quaternion Index exceed..");
			return FLT_MAX;

		}




		static JQuaternion ToQuaternion(const JVector3& euler);
		static JQuaternion ToQuaternion(float pitch, float yaw, float roll);

		inline static JQuaternion Identity()
		{
			JQuaternion q;
			q.SetSIMD(DirectX::XMQuaternionIdentity());
			return q;
		}
		inline static JQuaternion Inverse(const JQuaternion& q)
		{
			JQuaternion result;
			result.SetSIMD(DirectX::XMQuaternionInverse(q.GetSIMD()));
			return result;
		}
		inline static JQuaternion Normalize(const JQuaternion& q)
		{
			JQuaternion result;
			result.SetSIMD(DirectX::XMQuaternionNormalize(q.GetSIMD()));
			return result;
		}
		inline static JQuaternion Slerp(const JQuaternion& q1, const JQuaternion& q2, float t)
		{
			JQuaternion result;
			result.SetSIMD(DirectX::XMQuaternionSlerp(q1.GetSIMD(), q2.GetSIMD(), t));
			return result;
		}
		static JVector3 ToEuler(const JQuaternion& q);
		static JQuaternion RotationRollPitchYawFromVector(const JVector3& angles);
		static void ToAxisAngle(JVector3& out_axis, f32& out_angle, const JQuaternion& q);
		static JQuaternion RotatationAxis(const JVector3& axis, float angle);
		static JQuaternion FromTwoVectors(const JVector3& u, const JVector3& v);
	private:
		inline DirectX::XMVECTOR GetSIMD() const
		{
			DirectX::XMFLOAT4 value(x, y, z, w);
			return DirectX::XMLoadFloat4(&value);
		}
		inline void SetSIMD(DirectX::XMVECTOR q)
		{
			DirectX::XMFLOAT4 value;
			DirectX::XMStoreFloat4(&value, q);

			x = value.x;
			y = value.y;
			z = value.z;
			w = value.w;
		}

	};
}
