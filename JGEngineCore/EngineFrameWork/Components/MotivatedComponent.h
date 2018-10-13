#pragma once
#include"Component.h"


/*
Class : MotivatedComponent  클래스( 계층구조를 위한 컴포넌트 ) */
class ENGINE_EXPORT MotivatedComponent : public Component
{
public:
	MotivatedComponent();
	virtual ~MotivatedComponent();
	bool m_bDefaultView = false;
protected:
	virtual const JGMatrix& GetViewMatrix();
	const JGMatrix& GetProjectionMatrix();
	const JGMatrix& GetOrthoMatrix();
public:
	void UseDefaultViewMatrix();
};