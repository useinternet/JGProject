#pragma once
#include"MotivatedComponent.h"



/*
Class : Motivated2DComponent 
Exp : 계층구조를 가지고있는 2D 컴포넌트 
@m unique_ptr<JGVector2D> m_Location : 위치 좌표
@m unique_ptr<JGAngle2D> m_Angle     : 각도 
@m unique_ptr<JGScale2D> m_Scale     : 스케일 
@m unique_ptr<JGMatrix>  m_WorldMatrix : 월드 상에 나타낼 값(위치,각도,스케일)을 연산하기위한 행렬 
@m unique_ptr<JGMatrix>  m_ScaleMatrix : 스케일 값을 나타낼 행렬 
@m unique_ptr<JGMatrix>  m_AngleMatrix : 각도 값을 나타낼 행렬 
@m unique_ptr<JGMatrix>  m_TranslationMatrix : 이동 값을 나타낼 행렬 
@m JGVector2D m_PrevLocation : 위치 좌표 이전 값
@m JGAngle2D m_PrevAngle  : 각도 이전 값
@m JGScale2D m_PrevScale : 스케일 이전 값
@m m_bChange : 위치, 각도, 스케일중 하나라도 바뀌었는지 여부 (Rendering용)
@m m_bIsChangeLocation : 위치 변동 여부 
@m m_bIsChangeAngle : 각도 변동 여부 
@m m_bIsChangeScale : 스케일 변동 여부 */
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
	bool m_bIsChangeLocation = false;
	bool m_bIsChangeAngle    = false;
	bool m_bIsChangeScale    = false;
public:
	Motivated2DComponent();
	virtual ~Motivated2DComponent();
	/*
	Exp : 컴포넌트가 생성되고 최초 실행되는 함수이다. ( 부모 클래스의 BeginComponent도 호출해야한다. ) */
	virtual void BeginComponent(World* world) override;

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
	Exp : 컴포넌트 월드 위치 값을 가져온다. */
	JGVector2D GetComponentWorldLocation();
	/*
	Exp : 컴포넌트 월드 각도 값을 가져온다. */
	JGAngle2D GetComponentWorldAngle();
	/*
	Exp : 컴포넌트 스케일 값을 가져온다. */
	JGScale2D& GetComponentScale();
	
	/*
	Exp : 위치 값이 바뀌었으면 true를 그대로이면 false를 반환한다. */
	bool IsChangeLocation();
	/*
	Exp : 각도 값이 바뀌었으면 true를 그대로이면 false를 반환한다. */
	bool IsChangeAngle();
	/*
	Exp : 스케일 값이 바뀌었으면 true를 그대b로이면 false를 반환한다. */
	bool IsChangeScale();

	/*
	Exp :*/
	JGVector2D* GetComponentLocationAddress();
	JGAngle2D*  GetComponentAngleAddress();

protected:
	/*
	Exp : 이 컴포넌트의 월드 행렬을 가져온다. */
	const JGMatrix& GetWorldMatrix();

private:
	virtual bool Render2DCurling();
};