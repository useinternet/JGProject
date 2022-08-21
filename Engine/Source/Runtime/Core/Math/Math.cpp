#include "Math/Math.h"
#include "Vector.h"


PVector3 PMath::ConvertToRadians(const PVector3& vDegrees)
{
	return PVector3(ConvertToRadians(vDegrees.x), ConvertToRadians(vDegrees.y), ConvertToRadians(vDegrees.z));
}

PVector3 PMath::ConvertToDegrees(const PVector3& vRadians)
{
	return PVector3(ConvertToDegrees(vRadians.x), ConvertToDegrees(vRadians.y), ConvertToDegrees(vRadians.z));
}