#include<PCH.h>
#include"Vector.h"
namespace Common
{
	JVector3 operator*(float k, const JVector3& v) {
		return v * k;
	}
	JVector3 operator/(float k, const JVector3& v) {
		return v / k;
	}



	float Dot(const JVector3& v1, const JVector3& v2) {
		return DirectX::XMVectorGetX(DirectX::XMVector3Dot(v1.GetSIMD(), v2.GetSIMD()));
	}
	JVector3 Normalize(const JVector3& v) {
		return JVector3(DirectX::XMVector3Normalize(v.GetSIMD()));
	}
	JVector3 Cross(const JVector3& v1, const JVector3& v2) {
		return JVector3(DirectX::XMVector3Cross(v1.GetSIMD(), v2.GetSIMD()));
	}

}
