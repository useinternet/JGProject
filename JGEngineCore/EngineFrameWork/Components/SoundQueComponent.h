#pragma once
#include"Component.h"
#include"../../SoundSystem/JGSound.h"





// 나중에 랜덤장치를 이용하여 효과음 을 여러개 넣어 랜덤 재생하게 만든다..( 추후 구현 )
class ENGINE_EXPORT SoundQueComponent : public Component
{
private:
	std::vector<std::shared_ptr<JGSound>> m_vSounds;



public:
	SoundQueComponent();
	virtual ~SoundQueComponent();
	bool AddSound(const std::string& SoundPath,ESoundMode mode = ESoundMode::Sound2D);

};