#pragma once
#include"StaticMesh2DComponent.h"
/*
Class : AnimationMesh2DComponent 
@m float m_AnimationDelay : 애니메이션 딜레이 값.
@m size_t m_TotalFrame  : 총 프레임
@m size_t m_WidthFrame  : 가로 프레임
@m size_t m_HeightFrame : 세로 프레임
@m float m_AccTime : 누적 시간
@m size_t m_CurrentFrame  : 현재 프레임
@m float m_IncreaseWidth  : 가로 1프레임 증가량
@m float m_IncreaseHeight : 세로 1프레임 증가량
@m float m_CurrentIncreaseWidth  : 현재 가로 프레임 증가량
@m float m_CurrentIncreaseHeight : 현재 세로 프레임 증가량 */
class ENGINE_EXPORT AnimationMesh2DComponent : public StaticMesh2DComponent
{
private:
	bool  m_bStop = false;
	float m_AnimationDelay = 0.0f;
	size_t m_TotalFrame    = 0;
	size_t m_WidthFrame    = 0;
	size_t m_HeightFrame   = 0;
private:
	float  m_AccTime = 0.0f;
	size_t m_CurrentFrame = 1;
	
	float m_IncreaseWidth  = 0.0f;
	float m_IncreaseHeight = 0.0f;
	float m_CurrentIncreaseWidth  = 0.0f;
	float m_CurrentIncreaseHeight = 0.0f;


	/// 애니메이션 1회용 변수
	bool m_AnimationEnd = false;
	std::unique_ptr<struct SAnimationIncrementBuffer_VS> m_AnimationBuffer;
public:
	AnimationMesh2DComponent();
	virtual ~AnimationMesh2DComponent();
	/*
	Exp : 셰이더 상수버퍼의 파라미터들을 설정한다. ( 상속 받아서 쓰세요 / 부모 함수 호출 안해도되요..) */
	virtual void ShaderParamSetting(JGShaderData* Data);
	/// Component클래스 참조 ***
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
	/*
	Exp : 애니메이션 셋팅을 초기화한다. */
	void InitAnimationSetting();


	void Stop();
	void Play();
	bool IsPlaying();
	void SetCurrentFrame(const size_t CurrentFrame);
	/// StaticMesh2DComponent 클래스 참조...
	virtual void SetConstructObject(ConstructObject* Object) override;
private:
	/// 애니메이션 프레임 계산..
	void AnimationCalulation();
};