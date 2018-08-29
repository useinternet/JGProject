#pragma once
#include"Component.h"


/*
Class : MotivatedComponent  클래스( 계층구조를 위한 컴포넌트 ) */
class ENGINE_EXPORT MotivatedComponent : public Component
{
private:
	// 얼레 GetWorld()로 불러와 메인 카메라의 뷰 행렬을 사용해야하지만..
	// 일단 임시로 만들어서 쓰자 구조가 아직 안잡혀있으니...
	std::unique_ptr<JGMatrix> m_ViewMatrix;
public:
	MotivatedComponent();
	virtual ~MotivatedComponent();


protected:
	const JGMatrix& GetViewMatrix();
	const JGMatrix& GetProjectionMatrix();
	const JGMatrix& GetOrthoMatrix();
private:
	void TempViewMatrixInit();

};