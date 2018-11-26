#pragma once
#include"Light/DirectionLight.h"
#include"Light/PointLight.h"
#include"Light/SpotLight.h"

namespace JGRC
{
	class CORE_EXPORT LightPassLightManager
	{
		typedef std::vector<PointLight> PointLightArray;
		typedef std::vector<SpotLight>  SpotLightArray;
	private:
		uint m_MaxDirectionLightCount = 1;
		uint m_MaxPointLightCount = 20;
		uint m_MaxSpotLightCount = 10;
		int m_DirectionLightCount = 0;
		int m_PointLightCount = 0;
		int m_SpotLightCount = 0;
		int m_TempCount2 = 0;
	private:
		// ∂Û¿Ã∆Æ
		std::unique_ptr<DirectionLight> m_DirectionLight;
		PointLightArray m_PointLightArray;
		SpotLightArray  m_SpotLightArray;
		class JGShaderArray* m_Shader = nullptr;
	private:
		LightPassLightManager(const LightPassLightManager& copy) = delete;
		LightPassLightManager(LightPassLightManager&& lm) = delete;
		LightPassLightManager& operator=(const LightPassLightManager& copy) = delete;
		LightPassLightManager& operator=(LightPassLightManager&& lm) = delete;
	public:
		LightPassLightManager();
		~LightPassLightManager();
		void InitManager(class JGShaderArray* Shader);
		void ParamSetting();
	public:
		DirectionLight * GetDirectionLight();
		DirectionLight*  AddDirectionLight();
		PointLight*      AddPointLight();
		SpotLight*       AddSpotLight();
		SpotLight*       GetTestSpotLight();
		void             DeletePointLight(PointLight* light);
		void             DeleteSpotLight(SpotLight* light);
		uint GetMaxPointLightCount()     { return m_MaxPointLightCount; }
		uint GetMaxSpotLightCount()      { return m_MaxSpotLightCount; }
		uint GetMaxDirectionLightCount() { return m_MaxDirectionLightCount; }
		uint GetMaxTotalLightCount() { 
			return m_MaxPointLightCount + m_MaxSpotLightCount + m_MaxDirectionLightCount; }
		uint GetCurrentTotalLightCount() {
			return m_DirectionLightCount + m_PointLightCount + m_SpotLightCount; }
	};
}