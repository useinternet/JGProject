#include"SoundSystem.h"
#include"../EngineStatics/JGLog.h"
using namespace std;
SoundSystem::SoundSystem()
{

}
SoundSystem::~SoundSystem()
{
	if (m_SoundDevice)
	{
		m_SoundDevice->close();
		m_SoundDevice = nullptr;
	}

}

bool SoundSystem::CreateSoundSystem()
{
	FMOD_RESULT result = FMOD_RESULT::FMOD_OK;

	// 사운드 장치 생성
	result = FMOD::System_Create(&m_SoundDevice);
	if (result != FMOD_RESULT::FMOD_OK)
	{
		JGLog::Write(ELogLevel::Error, TT("Create SoundDevice Failed."));
		return false;
	}
	// 버전 체크
	m_SoundDevice->getVersion(&m_Version);
	if (m_Version < FMOD_VERSION)
	{
		JGLog::Write(ELogLevel::Error,
			TT("You are using old version of FMOD %08x, This program requires %08x"), m_Version, FMOD_VERSION);
		return false;
	}
	// 장치 초기화
	result = m_SoundDevice->init(MaxChannel, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED,nullptr);
	if (result != FMOD_RESULT::FMOD_OK)
	{
		JGLog::Write(ELogLevel::Error, TT("SoundDevice Init Failed."));
		return false;
	}
	return true;
}
void SoundSystem::Tick()
{
	m_SoundDevice->update();
}
FMOD::System* SoundSystem::GetSoundDevice()
{
	return m_SoundDevice;
}

