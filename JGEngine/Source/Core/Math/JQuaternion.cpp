#include "pch.h"
#include "JQuaternion.h"
#include "JMatrix.h"
#include "JVector.h"
using namespace std;


namespace JG
{
	JQuaternion::JQuaternion(const JVector3& v)
	{
		ToQuaternion(v);
	}
	JQuaternion::JQuaternion(float pitch, float yaw, float roll)
	{
		ToQuaternion({ pitch, yaw, roll });
	}
	// 
	JQuaternion JQuaternion::ToQuaternion(const JVector3& euler)
	{
		//float c1 = cosf(euler.x / 2);
		//float c2 = cosf(euler.y / 2);
		//float c3 = cosf(euler.z / 2);

		//float s1 = sinf(euler.x / 2);
		//float s2 = sinf(euler.y / 2);
		//float s3 = sinf(euler.z / 2);


		/*

		*/
		JQuaternion q;
		//q.w = c3 * c1 * c2 + s3 * s1 * s2;
		//q.x = s3 * c1 * c2 - c3 * s1 * s2;
		//q.y = c3 * s1 * c2 + s3 * c1 * s2;
		//q.z = c3 * c1 * s2 - s3 * s1 * c2;


		//q.x = s1 * c2 * c3 + c1 * s2 * s3,
		//q.y = c1 * s2 * c3 - s1 * c2 * s3,
		//q.z = c1 * c2 * s3 + s1 * s2 * c3,
		//q.w = c1 * c2 * c3 - s1 * s2 * s3;


		q.SetSIMD(DirectX::XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z));
		return JQuaternion::Normalize(q);
	}
	JQuaternion JQuaternion::ToQuaternion(float pitch, float yaw, float roll)
	{
		return ToQuaternion({ pitch, yaw, roll });
	}

	//JVector3 JQuaternion::ToEuler(const JQuaternion& q)
	//{
	//	
	//
	//	double x = 0.0, y = 0.0, z = 0.0, w = 0.0;
	//	x = q.x; y = q.y; z = q.z; w = q.w;
	//
	//	double sqx = x * x;
	//	double sqy = y * y;
	//	double sqz = z * z;
	//	double sqw = w * w;
	//
	//	float one = (float)(atan2(2.0 * (y * z + x * w), (-sqx - sqy + sqz + sqw)) * JG_PI); 
	//	float two = (float)(asin(-2 * (x * z - y * w)) * JG_PI);
	//	float three = (float)(atan2(2.0f * (x * y + z * w), (sqx - sqy - sqz + sqw)) * JG_PI);
	//
	//	return JVector3(
	//		(float)(atan2(2.0 * (y * z + x * w), (-sqx - sqy + sqz + sqw)) * JG_PI),
	//		(float)(asin(-2 * (x * z - y * w)) * JG_PI),
	//		(float)(atan2(2.0f * (x * y + z * w), (sqx - sqy - sqz + sqw)) * JG_PI)
	//
	//	);
	//
	//	//float angle = 0.0;
	//	//DirectX::XMVECTOR simVec;
	//	//DirectX::XMQuaternionToAxisAngle(&simVec, &angle, JQuaternion::Normalize(q).GetSIMD());
	//	//
	//	//return 	JVector3::ConvertJVector3(simVec) * angle;
	//}


	JQuaternion JQuaternion::RotatationAxis(const JVector3& axis, float angle)
	{
		JQuaternion q;
		DirectX::XMFLOAT3 v_axis = { axis.x, axis.y, axis.z };
		q.SetSIMD(DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&v_axis), angle));
		return q;
	}

}


