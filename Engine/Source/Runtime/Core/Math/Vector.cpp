#include "Vector.h"
#include "Color.h"
#include "Matrix.h"

PVector4::PVector4(const PLinearColor& c) :
	x(c.R), y(c.G), z(c.B), w(c.A) {}

PVector4::PVector4(const PVector3& v, float32 f) :
	x(v.x), y(v.y), z(v.z), w(f) {}

PVector2 operator* (float k, const PVector2& v)
{
	return v * k;
}

PVector3::PVector3(const PLinearColor& color) :
	x(color.R), y(color.G), z(color.B) {}


PVector3 operator* (float k, const PVector3& v)
{
	return v * k;
}

PVector4 operator*(float k, const PVector4& v)
{
	return v * k;
}

PVector3 PVector3::UnProject(const PVector3& v, const PMatrix& proj, const PMatrix& view, const PMatrix& world, const PVector2& viewPortXY, const PVector2& viewportSize, float32 minZ, float32 maxZ)
{
	auto simVec = DirectX::XMVector3Unproject(PVector3::GetSIMD(v), viewPortXY.x, viewPortXY.y, viewportSize.x, viewportSize.y, minZ, maxZ, proj.GetSIMD(), view.GetSIMD(), world.GetSIMD());

	return ConvertPVector3(simVec);
}