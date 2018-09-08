#pragma once
#include"Component.h"
#include"../../SoundSystem/JGSound.h"


class ENGINE_EXPORT SoundComponent : public Component
{
private:
	std::unique_ptr<JGSound> m_Sound;
	bool m_Paused = false;
	float Volume = 1.0f;
public:
	SoundComponent();
	virtual ~SoundComponent();
	bool CreateSound(const std::string& SoundPath,const ESoundMode mode);
	void Play();
	void Pause();
	void Stop();
	bool IsPause();
	bool IsPlaying();
	void SetVolume(const float Volume);
	void AddVolume(const float Volume);
};