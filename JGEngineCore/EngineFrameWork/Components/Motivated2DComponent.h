#pragma once
#include"MotivatedComponent.h"




class ENGINE_EXPORT Motivated2DComponent : public MotivatedComponent
{
	std::unique_ptr<JGVector2D> m_Location;
	std::unique_ptr<JGAngle2D>  m_Angle;
	std::unique_ptr<JGScale2D>  m_Scale;
	std::unique_ptr<JGMatrix>   m_WorldMatrix;


	std::unique_ptr<JGMatrix>   m_ScaleMatrix;
	std::unique_ptr<JGMatrix>   m_AngleMatrix;
	std::unique_ptr<JGMatrix>   m_TranslationMatrix;
	JGVector2D m_PrevLocation;
	JGAngle2D m_PrevAngle;
	JGScale2D m_PrevScale;
	bool m_bChange = true;
public:
	Motivated2DComponent();
	virtual ~Motivated2DComponent();
	/*
	Exp : 컴포넌트가 생성되고 최초 실행되는 함수이다. ( 부모 클래스의 BeginComponent도 호출해야한다. ) */
	virtual void BeginComponent() override;

	/*
	Exp : 매 프레임 한번 실행되는 함수이다. ( 부모 클래스의 Tick도 호출해야한다. ) */
	virtual void Tick(const float DeltaTime) override;
	/*
	Exp : 컴포넌트에 위치 값을 더한다. */
	void AddComponentLocation(const float x, const float y);
	/*
	Exp : 컴포넌트에 위치 값을 더한다. */
	void AddComponentLocation(JGVector2D& vector);
	/*
	Exp : 컴포넌트에 각도 값을 더한다. */
	void AddComponentAngle(const float angle);
	/*
	Exp : 컴포넌트에 각도 값을 더한다. */
	void AddComponentAngle(JGAngle2D& angle);
	/*
	Exp : 컴포넌트에 스케일 값을 더한다. */
	void AddComponentScale(const float flatNum);
	/*
	Exp : 컴포넌트에 스케일 값을 더한다. */
	void AddComponentScale(const float x, const float y);
	/*
	Exp : 컴포넌트에 스케일 값을 더한다. */
	void AddComponentScale(JGScale2D& scale);
	/*
	Exp : 컴포넌트 지역 위치 값을 대입한다. */
	void SetComponentLocation(const float x, const float y);
	/*
	Exp : 컴포넌트 지역 위치 값을 대입한다. */
	void SetComponentLocation(const JGVector2D& vector);
	/*
	Exp : 컴포넌트 지역 각도 값을 대입한다. */
	void SetComponentAngle(const float angle);
	/*
	Exp : 컴포넌트 지역 각도 값을 대입한다. */
	void SetComponentAngle(const JGAngle2D& angle);
	/*
	Exp : 컴포넌트 지역 스케일 값을 대입한다. */
	void SetComponentScale(const float flatNum);
	/*
	Exp : 컴포넌트 지역 스케일 값을 대입한다. */
	void SetComponentScale(const float x, const float y);
	/*
	Exp : 컴포넌트 지역 스케일 값을 대입한다. */
	void SetComponentScale(const JGScale2D& scale);

	/*
	Exp : 컴포넌트 지역 위치 값을 가져온다. */
	JGVector2D& GetComponentLocalLocation();
	/*
	Exp : 컴포넌트 지역 각도 값을 가져온다. */
	JGAngle2D& GetComponentLocalAngle();
	/*
	Exp : 컴포넌트 지역 스케일 값을 가져온다. */
	JGScale2D& GetComponentLocalScale();
	/*
	Exp : 컴포넌트 월드 위치 값을 가져온다. */
	JGVector2D GetComponentWorldLocation();
	/*
	Exp : 컴포넌트 월드 각도 값을 가져온다. */
	JGAngle2D GetComponentWorldAngle();
	/*
	Exp : 컴포넌트 월드 스케일 값을 가져온다. */
	JGScale2D GetComponentWorldScale();


protected:
	const JGMatrix& GetWorldMatrix();
};