#pragma once
#include "SE.h"
#include "DataType/Abstract/EngineCore.h"


namespace SE
{
	class SOUNDENGINE_API SoundEngine : public EngineCore
	{
	public:
		SoundEngine(const GlobalLinkStream& stream);
		virtual ~SoundEngine() = default;
	public:
		virtual void Update() override;
		virtual void OnEvent(Event& e) override;
	};
}