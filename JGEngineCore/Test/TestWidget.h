#pragma once
#include"../EngineFrameWork/Object/UserWidget.h"


class ENGINE_EXPORT TestWidget : public UserWidget
{
private:
	class Button* TestButton;
	class TextComponent* ButtonPos;
	class Text* MusicText;
	class SoundComponent* sound;
public:
	TestWidget();
	virtual ~TestWidget();
	virtual void Tick(const float DeltaTime) override;
};