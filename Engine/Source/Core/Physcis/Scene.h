#pragma once
#include "PhysicsObject.h"


namespace JG
{
	namespace Physics
	{
		class IRigidBody;
		class IScene : public PhysicsObject
		{
			JGCLASS
		public:
			virtual void AddObject(SharedPtr<IRigidBody> _object)         = 0;
			virtual void AddObjects(List<SharedPtr<IRigidBody>> _objects) = 0;
			virtual void RemoveObject(SharedPtr<IRigidBody> _object)      = 0;
			virtual void RemoveObjects(SharedPtr<IRigidBody> _objects)    = 0;
			virtual void ClearObjects() = 0;
		public:
			virtual void Simulate() = 0;
		};
	}
}