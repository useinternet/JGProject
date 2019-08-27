#pragma once

#include "GFW.h"
#include "DataType/Abstract/EngineCore.h"


namespace GFW
{
	class GAMEFRAMEWORK_API Game : public EngineCore
	{
	public:
		Game(const GlobalLinkStream& stream);
		virtual ~Game() = default;
	public:
		virtual void Load() override {}
		virtual void Update() override;
		virtual void OnEvent(Event& e) override;
	};
}
