#pragma once
#include"../EngineStatics/Engine.h"


enum class ESoundMode
{
	Stream2D,Sound2D, Stream2D_Loop, Sound2D_Loop
};
class ENGINE_EXPORT JGSound
{
private:
	FMOD::Sound* m_Sound  = nullptr;
	FMOD::Channel* m_Channel = nullptr;
	ESoundMode m_SoundMode;
	float m_Volume = 100.0f;

	FMOD::System* m_pSystem = nullptr;
public:
	JGSound();
	~JGSound();
	bool CreateSound(FMOD::System* System,const ESoundMode Mode,const std::string& SoundPath);
	void Play();
	void Pause(bool pause);
	void Stop();
	bool IsPause();
	bool IsPlaying();
	void SetVolume(const float Volume);
	void AddVolume(const float Volume);
};