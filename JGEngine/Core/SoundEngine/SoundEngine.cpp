#include "pch.h"
#include "SoundEngine.h"
#include "Class/Log.h"
using namespace std;

namespace SE
{
	SoundEngine::SoundEngine(const GlobalLinkStream& stream)
		: EngineCore(stream)
	{
		

		SE_LOG_INFO("SoundEngine Init Compelete");
	}

	void SoundEngine::Update()
	{
	
	}
	void SoundEngine::OnEvent(Event& e)
	{

	}
}