#include "PCH/PCH.h"
#include "Vector.h"
#include "Color.h"
#include "Matrix.h"

HVector4::HVector4(const HLinearColor& c) :
	x(c.R), y(c.G), z(c.B), w(c.A) {}

HVector4::HVector4(const HVector3& v, float32 f) :
	x(v.x), y(v.y), z(v.z), w(f) {}

HVector2 operator* (float k, const HVector2& v)
{
	return v * k;
}

HVector3::HVector3(const HLinearColor& color) :
	x(color.R), y(color.G), z(color.B) {}


HVector3 operator* (float k, const HVector3& v)
{
	return v * k;
}

HVector4 operator*(float k, const HVector4& v)
{
	return v * k;
}

HVector3 HVector3::UnProject(const HVector3& v, const HMatrix& proj, const HMatrix& view, const HMatrix& world, const HVector2& viewPortXY, const HVector2& viewportSize, float32 minZ, float32 maxZ)
{
	auto simVec = DirectX::XMVector3Unproject(HVector3::GetSIMD(v), viewPortXY.x, viewPortXY.y, viewportSize.x, viewportSize.y, minZ, maxZ, proj.GetSIMD(), view.GetSIMD(), world.GetSIMD());

	return ConvertPVector3(simVec);
}