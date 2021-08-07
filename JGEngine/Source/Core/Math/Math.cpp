#include "pch.h"
#include "Math.h"
#include "JVector.h"
namespace JG
{
	JVector3 Math::ConvertToRadians(const JVector3& vDegrees)
	{
		return JVector3(ConvertToRadians(vDegrees.x), ConvertToRadians(vDegrees.y), ConvertToRadians(vDegrees.z));
	}
	JVector3 Math::ConvertToDegrees(const JVector3& vRadians)
	{
		return JVector3(ConvertToDegrees(vRadians.x), ConvertToDegrees(vRadians.y), ConvertToDegrees(vRadians.z));
	}
}