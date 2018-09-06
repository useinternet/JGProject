#pragma once
#include"StaticMesh2DComponent.h"
/*
Class : AnimationMesh2DComponent 
@m float m_AnimationDelay : 애니메이션 딜레이 값.
@m size_t m_TotalFrame : 총 프레임
@m	size_t m_WidthFrame : 가로 프레임
@m	size_t m_HeightFrame : 세로 프레임
@m	bool m_IsRepeat : 애니메이션 반복 여부
@m	size_t m_RepeatStartFrame : 반복 시작 구간
@m	size_t m_RepeatEndFrame : 반복 끝 구간
@m	size_t m_RepeatCount : 반복 횟수(-1이면 무한반복)
@m float m_AccTime : 누적 시간
@m size_t m_CurrentFrame : 현재 프레임
@m float m_IncreaseWidth : 가로 1프레임 증가량
@m float m_IncreaseHeight : 세로 1프레임 증가량
@m float m_CurrentIncreaseWidth : 현재 가로 프레임 증가량
@m float m_CurrentIncreaseHeight : 현재 세로 프레임 증가량 */
class ENGINE_EXPORT AnimationMesh2DComponent : public StaticMesh2DComponent
{
private:
	float m_AnimationDelay = 0.0f;
	size_t m_TotalFrame  = 0;
	size_t m_WidthFrame  = 0;
	size_t m_HeightFrame = 0;
private:
	float m_AccTime = 0.0f;
	size_t m_CurrentFrame = 1;
	
	float m_IncreaseWidth = 0.0f;
	float m_IncreaseHeight = 0.0f;
	float m_CurrentIncreaseWidth  = 0.0f;
	float m_CurrentIncreaseHeight = 0.0f;
	
	bool m_AnimationEnd = false;
	std::unique_ptr<struct SAnimationIncrementBuffer_VS> m_AnimationBuffer;
public:
	AnimationMesh2DComponent(const AnimationMesh2DComponent& copy) = delete;
	AnimationMesh2DComponent();
	~AnimationMesh2DComponent();
	/*
	Exp : 셰이더 상수버퍼의 파라미터들을 설정한다. ( 상속 받아서 쓰세요 / 부모 함수 호출 안해도되요.. */
	virtual void ShaderParamSetting(JGShaderData* Data);

	virtual void Tick(const float DeltaTime) override;
	/*
	Exp : 애니메이션 딜레이 값 설정 */
	void AnimationSetDelay(const float Delay);
	/*
	Exp : 애니메이션이 끝낫을때.. */
	bool AnimationIsEnd();
	/*
	Exp : 해당 프레임일떄 */
	bool AnimationIsFrame(const size_t Frame);


	virtual void SetConstructObject(ConsructObject* Object) override;
private:
	void AnimationCalulation();
};