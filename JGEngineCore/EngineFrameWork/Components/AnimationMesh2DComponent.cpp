#include"AnimationMesh2DComponent.h"
#include"../../EngineStatics/JGConstructHelper.h"
#include"../../RenderSystem/ShaderCode/HLSLConstantBufferDesc.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGShaderData.h"
#include"../../RenderSystem/JGMaterial/JG2DMesh.h"
using namespace std;
AnimationMesh2DComponent::AnimationMesh2DComponent()
{
	RegisterComponentID(typeid(this));
	m_AnimationBuffer = make_unique<SAnimationIncrementBuffer_VS>();
	m_AnimationDelay = 0.2f;
}

AnimationMesh2DComponent::~AnimationMesh2DComponent()
{
}
void AnimationMesh2DComponent::ShaderParamSetting(JGShaderData * Data)
{
	StaticMesh2DComponent::ShaderParamSetting(Data);
	Data->InsertData(TT("AnimationIncrementBuffer"), m_AnimationBuffer.get());

	// 셰이더 ...데이터 작성
}
void AnimationMesh2DComponent::Tick(const float DeltaTime)
{
	StaticMesh2DComponent::Tick(DeltaTime);
	m_AccTime += DeltaTime;
	if (m_AccTime >= m_AnimationDelay)
	{
		m_CurrentFrame++;
		m_AccTime = 0.0f;
		AnimationCalulation();
		m_AnimationBuffer->WidthIncrement = m_CurrentIncreaseWidth;
		m_AnimationBuffer->HeightIncrement = m_CurrentIncreaseHeight;
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

void AnimationMesh2DComponent::SetConstructObject(ConstructObject* Object)
{
	AnimationMesh2DObject* object = dynamic_cast<AnimationMesh2DObject*>(Object);
	if (object)
	{
		SetTexture(object->Texture.get());
		SetMesh(object->Mesh.get());
		SetShaderName(object->ShaderName);
		SetPivot(object->Pivot.get());
		m_TotalFrame = object->TotalFrame;
		m_WidthFrame = object->WidthFrame;
		m_HeightFrame = object->HeightFrame;
		m_IncreaseWidth = object->IncreaseWidth;
		m_IncreaseHeight = object->IncreaseHeight;
		
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
