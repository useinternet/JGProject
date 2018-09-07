#pragma once
#include"../EngineStatics/Engine.h"
class JGSound;

class ENGINE_EXPORT SoundSystem
{
	const unsigned int MaxChannel = 100;
	typedef unsigned int FMODVersion;
private:
	FMOD::System* m_SoundDevice = nullptr;
	FMODVersion   m_Version;
public:
	SoundSystem();
	~SoundSystem();
	bool CreateSoundSystem();
	void Tick();
	FMOD::System* GetSoundDevice();
};