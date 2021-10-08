#pragma once

#include "JGCore.h"



namespace JG
{
	struct PhysicsSceneDesc
	{
		JVector3 Gravity = 0.0f;

	};


	enum class EGeometryType
	{
		Box,
		Sphere,
	};


	class PhysicsShapeDesc
	{
	public:
		EGeometryType GeometryType;

		f32 StaticFriction  = 0.0f;
		f32 DynamicFriction = 0.0f;
		f32 Restitution     = 0.0f;
	public:
		virtual ~PhysicsShapeDesc() = default;
	};

	class PhysicsBoxDesc : public PhysicsShapeDesc
	{
	public:
		f32 HalfX = 0.0f;
		f32 HalfY = 0.0f;
		f32 HalfZ = 0.0f;
	public:
		virtual ~PhysicsBoxDesc() = default;
	};

	class PhysicsSphereDesc : public PhysicsShapeDesc
	{
	public:
		f32 R = 0.0f;
	public:
		virtual ~PhysicsSphereDesc() = default;
	};


}