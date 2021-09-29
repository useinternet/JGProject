#pragma once
#include "Physcis/Scene.h"


namespace JG
{
	namespace Physics
	{
		class IRigidBody;
	}

	class PhysXScene : public Physics::IScene
	{
		JGCLASS
	public:
		PhysXScene(const Physics::SceneDesc& sceneDesc);
	public:
		virtual void AddObject(SharedPtr<Physics::IRigidBody> _object) override;
		virtual void AddObjects(List<SharedPtr<Physics::IRigidBody>> _objects) override;
		virtual void RemoveObject(SharedPtr<Physics::IRigidBody> _object) override;
		virtual void RemoveObjects(SharedPtr<Physics::IRigidBody> _objects) override;
		virtual void ClearObjects() override;
	public:
		virtual void Simulate() override;
	};
}