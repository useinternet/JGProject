#pragma once
#include"EngineFrameWork/Object/UserWidget.h"



class StartMainWidget : public UserWidget
{
private:

	class ImageBox* MainBackGround;
	class Button*   StartButton;
	class Button*   ConfigButton;
	class Button*   ExpButton;
	class Text*     GameTitle;
	class Text*     GameStart;
	class Text*     GameExp;
	class Text*     GameConfig;

	bool m_ButtonClick = false;
	bool m_PrevButtonClick = false;
public:
	StartMainWidget();
	virtual ~StartMainWidget();


	virtual void Tick(const float DeltaTime) override;

	void ChangeTextLocationWithButtonClick(Button* button,Text* text,const JGVector2D& TextLocation);

};