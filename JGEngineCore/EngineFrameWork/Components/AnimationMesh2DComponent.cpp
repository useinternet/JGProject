#include"AnimationMesh2DComponent.h"

AnimationMesh2DComponent::AnimationMesh2DComponent()
{
}

AnimationMesh2DComponent::~AnimationMesh2DComponent()
{
}
void AnimationMesh2DComponent::Tick(const float DeltaTime)
{
	StaticMesh2DComponent::Tick(DeltaTime);

	if (FirstTick)
	{
		FirstTick = false;
		return;
	}
	m_AccTime += DeltaTime;
	if (m_AccTime >= m_AnimationDelay)
	{
		m_AccTime = 0.0f;
		AnimationCalulation();
	}
}
void AnimationMesh2DComponent::Render()
{
	Component::Render();
	m_CurrentFrame++;



}
void AnimationMesh2DComponent::SetAnimation(const size_t TotalFrame, const size_t WidthFrame, const size_t HeightFrame, const float AnimationDelay,
	const float TextureWidth, const float TextureHeight)
{
	m_AnimationDelay = AnimationDelay;
	m_TotalFrame = TotalFrame;
	m_WidthFrame = WidthFrame;
	m_HeightFrame = m_HeightFrame;


	m_IncreaseWidth = TextureWidth / (float)WidthFrame;
	m_IncreaseHeight = TextureHeight / (float)HeightFrame;
}

void AnimationMesh2DComponent::AnimationRepeatOn(const size_t StartFrame, const size_t EndFrame, const size_t RepeatCount)
{
	m_IsRepeat = true;
	m_RepeatStartFrame = StartFrame;
	m_RepeatEndFrame = EndFrame;
	m_RepeatCount = RepeatCount;
}
void AnimationMesh2DComponent::AnimationCalulation()
{
	// 현제 프레임 % 가로프레임 == 1
	if (m_CurrentFrame % m_WidthFrame == 1)
	{
		m_CurrentIncreaseWidth = 0.0f;
		m_CurrentIncreaseHeight = m_IncreaseHeight;
	}
	else
	{
		m_CurrentIncreaseHeight = 0.0f;
		m_CurrentIncreaseWidth = m_IncreaseWidth;
	}
}
