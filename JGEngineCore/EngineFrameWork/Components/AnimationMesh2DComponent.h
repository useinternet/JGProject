#pragma once
#include"StaticMesh2DComponent.h"


class ENGINE_EXPORT AnimationMesh2DComponent : public StaticMesh2DComponent
{
private:
	// 애니메이션 종류 
	// 1. 한장에 다떄려박는 애니메이션
	// 2. 한장 한장 애니메이션

	// 애니메이션 딜레이..


	// - 한장에 다떄려 박는 애니메이션에 필요한 변수
	// 총 프레임 / 가로 프레임 / 세로 프레임 수



	// - 한장 한장 애니메이션
	// 총 프레임
	// 애니메이션 딜레이..


	// 반복 애니메이션 에 필요한 변수(1을 맨처음이라 가정)
	// 첫 부분 / 마지막 반복 부분
	// 반복 횟수 (-1 이면 무한 반복)
	//
	bool FirstTick = true;

	float m_AnimationDelay = 0.0f;
	size_t m_TotalFrame  = 0;
	size_t m_WidthFrame  = 0;
	size_t m_HeightFrame = 0;

	bool m_IsRepeat = false;
	size_t m_RepeatStartFrame = 0;
	size_t m_RepeatEndFrame = 0;
	size_t m_RepeatCount = 0;

private:
	float m_AccTime = 0.0f;
	size_t m_CurrentFrame = 1;
	
	float m_IncreaseWidth = 0.0f;
	float m_IncreaseHeight = 0.0f;
	float m_CurrentIncreaseWidth  = 0.0f;
	float m_CurrentIncreaseHeight = 0.0f;
public:
	AnimationMesh2DComponent(const AnimationMesh2DComponent& copy) = delete;
	AnimationMesh2DComponent();
	~AnimationMesh2DComponent();

	virtual void Tick(const float DeltaTime) override;
	virtual void Render() override;
	void SetAnimation(const size_t TotalFrame, const size_t WidthFrame, const size_t HeightFrame, const float AnimationDelay,
		const float TextureWidth, const float TextureHeight);
	void AnimationRepeatOn(const size_t StartFrame, const size_t EndFrame, const size_t RepeatCount);

private:
	void AnimationCalulation();
};