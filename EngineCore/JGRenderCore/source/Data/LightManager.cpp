#include"LightManager.h"
#include"Scene.h"
#include"Shader/Shader.h"
#include"Shader/CommonShaderRootSignature.h"
#include"CommonData.h"
using namespace JGRC;
using namespace std;
using namespace DirectX;

void LightManager::BuildLight(CommonShaderRootSignature* CommonSig)
{
	if (m_DirLight)
		m_DirLight->Build();
	for (auto& iter : m_PointLights)
	{
		iter->Build();
	}
	for (auto& iter : m_SpotLights)
	{
		iter->Build();
	}
}
void LightManager::Update(FrameResource* CurrFrameResource)
{
	int Count = 0;
	PassData* MainPassData = CommonData::_Scene()->GetMainPass();
	MainPassData->Data.DirLightCount   = m_DirectionLightCount;
	MainPassData->Data.SpotLightCount  = m_SpotLightCount;
	MainPassData->Data.PointLightCount = m_PointLightCount;

	Count = 0;
	if (m_DirLight)
		m_DirLight->Update(Count++,CurrFrameResource);
	for (auto& iter : m_PointLights)
	{
		iter->Update(Count++, CurrFrameResource);
	}
	for (auto& iter : m_SpotLights)
	{
		iter->Update(Count++, CurrFrameResource);
	}

}
void LightManager::DrawShadowMap(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource)
{
	if (m_DirLight)
		m_DirLight->BuildShadowMap(CurrFrameResource, CommandList);
	for (auto& iter : m_PointLights)
	{
		iter->BuildShadowMap(CurrFrameResource, CommandList);
	}
	for (auto& iter : m_SpotLights)
	{
		iter->BuildShadowMap(CurrFrameResource, CommandList);
	}
}
JGLight* LightManager::AddLight(ELightType type, ELightExercise ExType)
{
	auto l = make_unique<JGLight>(type, ExType);
	JGLight* result = l.get();
	m_LightCount++;
	switch (type)
	{
	case ELightType::Direction:
		if (m_DirectionLightCount > 0)
			return nullptr;
		m_DirLight = move(l);
		m_DirectionLightCount++;
		break;
	case ELightType::Point:
		m_PointLightCount++;
		m_PointLights.push_back(move(l));
		break;
	case ELightType::Spot:
		m_SpotLightCount++;
		m_SpotLights.push_back(move(l));
		break;
	}
	return result;
}
JGLight* LightManager::DeleteLight(JGLight* light)
{
	return nullptr;
}
UINT LightManager::Size() const
{
	return m_LightCount;
}