#pragma once

#include"EngineFrameWork/Object/UserWidget.h"

class LoadingWidget : public UserWidget
{
private:
	class ImageBox* LoadingBackGroundImage;
	class LoadingComponent* LoadingSupporter;

	class Progressbar* SampleBar;
	bool ChangeWorld = false;
public:
	LoadingWidget();
	virtual ~LoadingWidget();

	virtual void BeginObject(World* world) override;
	virtual void Tick(const float DeltaTime) override;
};