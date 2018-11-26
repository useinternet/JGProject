#include"LightPassLightManager.h"
#include"MaterialSystem/Shader/JGShaderArray.h"
using namespace std;
using namespace JGRC;

LightPassLightManager::LightPassLightManager()
{
	m_DirectionLight = make_unique<DirectionLight>();
	for (uint i = 0; i < m_MaxPointLightCount; ++i)
	{
		PointLight l;
		m_PointLightArray.push_back(l);
	}
	for (uint i = 0; i < m_MaxSpotLightCount; ++i)
	{
		SpotLight l;
		m_SpotLightArray.push_back(l);
	}
}
LightPassLightManager::~LightPassLightManager()
{

}
void LightPassLightManager::InitManager(class JGShaderArray* Shader)
{
	m_Shader = Shader;
}
void LightPassLightManager::ParamSetting()
{
	if (m_Shader == nullptr)
	{
		JGLOG(log_Error, "JGRC::LightPassLightManager", "Binding Shader is nullptr")
		return;
	}
	if (m_DirectionLightCount == 1 && m_DirectionLight->IsDynamic())
	{
		m_Shader->Get(EShaderType::Pixel)->SetParam("DirLight", m_DirectionLight->GetData(), DirectionLight::ParamCount);
		m_DirectionLight->Static();
	}
	for (uint i = 0; i < m_PointLightCount; ++i)
	{
		if (m_PointLightArray[i].IsDynamic())
		{
			m_Shader->Get(EShaderType::Pixel)->SetParam("PntLight" + to_string((int)i), m_PointLightArray[i].GetData(), PointLight::ParamCount);
			m_PointLightArray[i].Static();
		}

	}
	for (uint i = 0; i < m_SpotLightCount; ++i)
	{
		if (m_SpotLightArray[i].IsDynamic())
		{
			m_Shader->Get(EShaderType::Pixel)->SetParam("SptLight" + to_string((int)i), m_SpotLightArray[i].GetData(), SpotLight::ParamCount);
			m_SpotLightArray[i].Static();
		}
	}
	m_Shader->Get(EShaderType::Pixel)->SetParam("DirectionLightCount", &m_DirectionLightCount, 1);
	m_Shader->Get(EShaderType::Pixel)->SetParam("PointLightCount", &m_PointLightCount, 1);
	m_Shader->Get(EShaderType::Pixel)->SetParam("SpotLightCount", &m_SpotLightCount, 1);
}
DirectionLight* LightPassLightManager::GetDirectionLight()
{
	return m_DirectionLight.get();
}
DirectionLight*  LightPassLightManager::AddDirectionLight()
{
	if (m_DirectionLightCount > 1)
	{
		JGLOG(log_Error, "JGRC::SRSLightPass", "DirectionLight is over 1 count")
			return nullptr;
	}
	m_DirectionLightCount++;
	return m_DirectionLight.get();
}
PointLight*      LightPassLightManager::AddPointLight()
{
	if (m_PointLightCount >= m_MaxPointLightCount)
	{
		JGLOG(log_Error, "JGRC::SRSLightPass", "Current PointLightCount is over MaxPointLightCount")
			return  nullptr;
	}
	return &m_PointLightArray[m_PointLightCount++];
}
SpotLight*       LightPassLightManager::AddSpotLight()
{
	if (m_SpotLightCount >= m_MaxSpotLightCount)
	{
		JGLOG(log_Error, "JGRC::SRSLightPass", "Current SpotLightCount is over MaxSpotLightCount")
			return  nullptr;
	}
	return &m_SpotLightArray[m_SpotLightCount++];
}
SpotLight*       LightPassLightManager::GetTestSpotLight()
{
	return &m_SpotLightArray[0];
}
void             LightPassLightManager::DeletePointLight(PointLight* light)
{
	bool find = false;
	for (int i = 0; i < m_PointLightCount; ++i)
	{
		if (&m_PointLightArray[i] == light)
		{
			find = true;
		}
		if (find && i < m_MaxPointLightCount - 1)
		{
			m_PointLightArray[i] = m_PointLightArray[i + 1];
		}
	}
	m_PointLightCount--;
}
void             LightPassLightManager::DeleteSpotLight(SpotLight* light)
{
	bool find = false;
	for (int i = 0; i < m_SpotLightCount; ++i)
	{
		if (&m_SpotLightArray[i] == light)
		{
			find = true;
		}
		if (find && i < m_MaxSpotLightCount - 1)
		{
			m_SpotLightArray[i] = m_SpotLightArray[i + 1];
		}
	}
	m_SpotLightCount--;
}