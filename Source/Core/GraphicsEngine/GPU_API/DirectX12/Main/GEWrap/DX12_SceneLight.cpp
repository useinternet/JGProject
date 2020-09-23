#include "pch.h"
#include "DX12_SceneLight.h"

#include "../GraphicsIF.h"
using namespace std;

namespace DX12
{

	GE::Texture* DX12_DirectionalLight::GetShadowMap() const
	{
		if (m_ShadowMaps == nullptr)
		{
			m_ShadowMaps = make_unique<DX12_Texture>();
		}

		return m_ShadowMaps.get();
	}

	GE::Texture* DX12_PointLight::GetShadowCubeMap() const
	{
		if (m_ShadowCubeMap == nullptr)
		{
			m_ShadowCubeMap = make_unique<DX12_Texture>();
		}


		return m_ShadowCubeMap.get();
	}


	GE::Texture* DX12_SpotLight::GetShadowMap() const
	{
		if (m_ShadowMap == nullptr)
		{
			m_ShadowMap = make_unique<DX12_Texture>();
		}


		return m_ShadowMap.get();
	}





}