#include"JGSound.h"
#include"../EngineStatics/JGLog.h"
JGSound::JGSound()
{


}
JGSound::~JGSound()
{
	if (m_Sound)
	{
		m_Sound->release();
		m_Sound = nullptr;
	}
}
bool JGSound::CreateSound(FMOD::System* System, const ESoundMode Mode, const std::string& SoundPath)
{
	FMOD_RESULT result = FMOD_RESULT::FMOD_OK;
	m_pSystem = System;
	switch (Mode)
	{
	case ESoundMode::Stream2D:
		result = m_pSystem->createStream(SoundPath.c_str(), FMOD_DEFAULT, nullptr, &m_Sound);
		break;
	case ESoundMode::Stream2D_Loop:
		result = m_pSystem->createStream(SoundPath.c_str(), FMOD_LOOP_NORMAL | FMOD_2D , nullptr, &m_Sound);
		break;
	case ESoundMode::Sound2D:
		result = m_pSystem->createSound(SoundPath.c_str(), FMOD_DEFAULT, nullptr, &m_Sound);
		break;
	case ESoundMode::Sound2D_Loop:
		result = m_pSystem->createSound(SoundPath.c_str(), FMOD_LOOP_NORMAL | FMOD_2D, nullptr, &m_Sound);
		break;
	}
	if (result != FMOD_RESULT::FMOD_OK)
	{
		JGLog::Write(ELogLevel::Error, TT("Create Sound(%s) Failed."), SoundPath);
		return false;
	}
	return true;
}
void JGSound::Play()
{
	m_pSystem->playSound(m_Sound, nullptr, false, &m_Channel);
	m_Channel->setVolume(m_Volume);
}
void JGSound::Pause(bool pause)
{
	if (m_Channel)
	{
		m_Channel->setPaused(pause);
	}
}
void JGSound::Stop()
{
	if (m_Channel)
	{
		m_Channel->stop();
	}
}
bool JGSound::IsPause()
{
	if (m_Channel)
	{
		bool result = true;
		m_Channel->getPaused(&result);
		return result;
	}
	return false;
}
bool JGSound::IsPlaying()
{
	if (m_Channel)
	{
		bool result = true;
		m_Channel->isPlaying(&result);
		return result;
	}
	return false;
}
void JGSound::SetVolume(const float Volume)
{
	m_Volume = Volume;
	if (m_Volume > 1.0f)
	{
		m_Volume = 1.0f;
	}
	else if (m_Volume < 0.0f)
	{
		m_Volume = 0.0f;
	}
	if (m_Channel)
	{
		m_Channel->setVolume(m_Volume);
	}
}
void JGSound::AddVolume(const float Volume)
{
	m_Volume += Volume;
	if (m_Volume > 1.0f)
	{
		m_Volume = 1.0f;
	}
	else if (m_Volume < 0.0f)
	{
		m_Volume = 0.0f;
	}
	if (m_Channel)
	{
		m_Channel->setVolume(m_Volume);
	}
}