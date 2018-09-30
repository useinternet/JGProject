#include"AnimationMesh2DComponent.h"
#include"../../EngineStatics/JGConstructHelper.h"
#include"../../RenderSystem/ShaderCode/HLSLConstantBufferDesc.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGShaderData.h"
#include"../../EngineStatics/JGLog.h"
using namespace std;
AnimationMesh2DComponent::AnimationMesh2DComponent()
{
	RegisterComponentID(typeid(this));
	m_AnimationBuffer = make_unique<SAnimationIncrementBuffer_VS>();
	m_AnimationDelay  = 0.2f;
}

AnimationMesh2DComponent::~AnimationMesh2DComponent()
{
}
void AnimationMesh2DComponent::ShaderParamSetting(JGShaderData * Data)
{
	StaticMesh2DComponent::ShaderParamSetting(Data);
	Data->InsertData(TT("AnimationIncrementBuffer"), m_AnimationBuffer.get());
}
void AnimationMesh2DComponent::Tick(const float DeltaTime)
{
	StaticMesh2DComponent::Tick(DeltaTime);
	float ReverseX = 0.0f;
	float ReverseY = 0.0f;
	switch (m_ReverseType)
	{
	case EReverse::Default:
		ReverseX = 0.0f;
		ReverseY = 0.0f;
		break;
	case EReverse::RL:
		ReverseX = -1.0f + m_IncreaseWidth;
		ReverseY = 0.0f;
		break;
	case EReverse::UD:
		ReverseX = 0.0f;
		ReverseY = -1.0f + m_IncreaseHeight;
		break;
	}
	m_AccTime += DeltaTime;
	if (m_AccTime >= m_AnimationDelay)
	{
		if (!m_bStop)
		{
			m_CurrentFrame++;
		}
	
		m_AccTime = 0.0f;
		AnimationCalulation();
		m_AnimationBuffer->WidthIncrement  = m_CurrentIncreaseWidth  + ReverseX;
		m_AnimationBuffer->HeightIncrement = m_CurrentIncreaseHeight + ReverseY;
	}
}
void AnimationMesh2DComponent::AnimationSetDelay(const float Delay)
{
	m_AnimationDelay = Delay;
}
bool AnimationMesh2DComponent::AnimationIsEnd()
{
	return m_AnimationEnd;
}
bool AnimationMesh2DComponent::AnimationIsFrame(const size_t Frame)
{
	if (m_CurrentFrame == Frame)
	{
		return true;
	}
	return false;
}
void AnimationMesh2DComponent::InitAnimationSetting()
{
	m_AnimationEnd = false;
	m_CurrentFrame = 1;
	m_CurrentIncreaseWidth = 0.0f;
	m_CurrentIncreaseHeight = 0.0f;
}
void AnimationMesh2DComponent::Stop()
{
	m_bStop = true;
}
void AnimationMesh2DComponent::Play()
{
	m_bStop = false;
}
bool AnimationMesh2DComponent::IsPlaying()
{
	if (m_bStop)
	{
		return false;
	}
	return true;
}
void AnimationMesh2DComponent::SetCurrentFrame(const size_t CurrentFrame)
{
	m_CurrentFrame = CurrentFrame;
}
void AnimationMesh2DComponent::Reverse(const EReverse reversetype)
{
	if (GetMesh())
	{
		m_AccTime = m_AnimationDelay;
		GetMesh()->MeshReverse(reversetype);
		m_ReverseType = reversetype;
	}
}
void AnimationMesh2DComponent::SetConstructObject(ConstructObject* Object)
{
	AnimationMesh2DObject* object = dynamic_cast<AnimationMesh2DObject*>(Object);
	if (object)
	{
		SetTexture(object->Texture.get());
		SetMesh(object->Mesh.get());
		SetShaderName(object->ShaderName);
		SetPivot(object->Pivot.get());

		m_TotalFrame     = object->TotalFrame;
		m_WidthFrame     = object->WidthFrame;
		m_HeightFrame    = object->HeightFrame;
		m_IncreaseWidth  = object->IncreaseWidth;
		m_IncreaseHeight = object->IncreaseHeight;
		m_ReverseType = object->ReverseType;
	}
}
void AnimationMesh2DComponent::AnimationCalulation()
{

	if (m_CurrentFrame > m_TotalFrame)
	{
		InitAnimationSetting();
		m_AnimationEnd = true;
		return;
	}

	m_AnimationEnd = false;

	// 현제 프레임 % 가로프레임 == 1 
	if (m_CurrentFrame % m_WidthFrame == 1)
	{
		m_CurrentIncreaseHeight = m_IncreaseHeight * ((m_CurrentFrame / m_WidthFrame) % m_HeightFrame);
	}
	m_CurrentIncreaseWidth = m_IncreaseWidth * ((m_CurrentFrame - 1) % m_WidthFrame);	
}
