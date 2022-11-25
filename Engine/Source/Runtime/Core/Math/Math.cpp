#include "PCH/PCH.h"
#include "Math/Math.h"
#include "Vector.h"


HVector3 HMath::ConvertToRadians(const HVector3& vDegrees)
{
	return HVector3(ConvertToRadians(vDegrees.x), ConvertToRadians(vDegrees.y), ConvertToRadians(vDegrees.z));
}

HVector3 HMath::ConvertToDegrees(const HVector3& vRadians)
{
	return HVector3(ConvertToDegrees(vRadians.x), ConvertToDegrees(vRadians.y), ConvertToDegrees(vRadians.z));
}