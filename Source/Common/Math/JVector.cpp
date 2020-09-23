#include "pch.h"
#include "JVector.h"
#include "Data/Color.h"
#include "JMatrix.h"

JVector4::JVector4(const JColor& c) :
	x(c.r), y(c.g), z(c.b), w(c.a) {}

JVector2 operator* (float k, const JVector2& v)
{
	return v * k;
}
JVector3 operator* (float k, const JVector3& v)
{
	return v * k;
}

JVector4 operator*(float k, const JVector4& v)
{
	return v * k;
}

JVector3 JVector3::UnProject(const JVector3& v, const JMatrix& proj, const JMatrix& view, const JMatrix& world, const JVector2& viewPortXY, const JVector2& viewportSize, float minZ, float maxZ)
{
	auto simVec = DirectX::XMVector3Unproject(JVector3::GetSIMD(v), viewPortXY.x, viewPortXY.y, viewportSize.x, viewportSize.y, minZ, maxZ, proj.GetSIMD(), view.GetSIMD(), world.GetSIMD());

	return ConvertJVector3(simVec);
}
