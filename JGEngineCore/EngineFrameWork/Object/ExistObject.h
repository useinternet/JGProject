#pragma once
#include"Object.h"

/*
Class : ExistObject 
Exp : 화면상에 그려지는 오브젝트.. */
class ExistObject : public Object
{
private:
	// 실험을 해보자..
	class StaticMesh2DComponent* Breath;
	class TextComponent* Frame;
public:
	ExistObject();
	virtual ~ExistObject();

	virtual void BeginObject() override;

	virtual void Tick(const float DeltaTime) override;
};