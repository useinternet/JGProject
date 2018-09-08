// ConsoleApplication1.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include"fmod.hpp"
#include"fmod.h"
#pragma comment(lib,"fmod64_vc.lib")
int main()
{
	FMOD::System*  system   = nullptr;
	FMOD::Sound*   sound    = nullptr;
	FMOD::Channel* channel  = nullptr;
	FMOD::System_Create(&system);
	unsigned int length = 0;
	FMOD_TIMEUNIT time = 0;
	system->init(1, FMOD_INIT_NORMAL, nullptr);
	system->createSound("Always-_2_.wav", FMOD_DEFAULT, nullptr, &sound);
	system->playSound(sound, nullptr, false, &channel);
	channel->setVolume(1.0f);
	sound->getLength(&length, time);
	while(1)
	{
		bool result = false;
		FMOD_RESULT Result = channel->isPlaying(&result);
		system->update();
	}
    return 0;
}

