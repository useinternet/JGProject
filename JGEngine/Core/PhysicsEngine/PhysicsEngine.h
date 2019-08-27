#pragma once
#include "PE.h"
#include "DataType/Abstract/EngineCore.h"


namespace PE
{
	class PHYSICSENGINE_API PhysicsEngine : public EngineCore
	{
	public:
		PhysicsEngine(const GlobalLinkStream& stream);
		virtual ~PhysicsEngine() = default;
	public:
		virtual void Load() override {}
		virtual void Update() override;
		virtual void OnEvent(Event& e) override;
	};
}