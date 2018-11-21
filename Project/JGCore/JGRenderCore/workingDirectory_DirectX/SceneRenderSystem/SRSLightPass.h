#pragma once
#include"SRSRenderTarget.h"
#include"SRSScene.h"
#include"Light/DirectionLight.h"
#include"Light/PointLight.h"
#include"Light/SpotLight.h"
#include"Camera/Camera.h"

// 셰이더 파일 내뿜을때 아닐때 나누기
// 라이트 따로 관리
// dx윈도우에 프리스텀 컬링 적용
// 큐브맵 적용
// 반사맵 적용
// 그림자 적용

namespace JGRC
{
	class CORE_EXPORT SRSLightPass
	{
	private:
		uint m_MaxPointLightCount = 20;
		uint m_MaxSpotLightCount  = 10;
		int m_DirectionLightCount = 0;
		int m_PointLightCount     = 0;
		int m_SpotLightCount      = 0;
		int m_TempCount2 = 0;


		typedef std::vector<PointLight> PointLightArray;
		typedef std::vector<SpotLight>  SpotLightArray;
	private:
		HWND m_hWnd;
		class DirectX* m_Dx;
		class JGShaderArray*  m_Shader;
		// 씬
		std::unique_ptr<SRSScene>        m_Scene;
		SRSRenderTarget* m_RenderTarget = nullptr;
		// 라이트
		std::unique_ptr<DirectionLight> m_DirectionLight;
		PointLightArray m_PointLightArray;
		SpotLightArray  m_SpotLightArray;
		// 카메라
		Camera* m_Camera = nullptr;
	private:
		SRSLightPass(const SRSLightPass& copy) = delete;
		SRSLightPass(SRSLightPass&& srp) = delete;
		SRSLightPass& operator=(const SRSLightPass& copy) = delete;
		SRSLightPass& operator=(SRSLightPass&& srp) = delete;
	public:
		SRSLightPass();
		virtual ~SRSLightPass();
		void Init(const DxWinConfig& config, SRSRenderTarget* SRSRT);
		// 나중에 라이트클래스도 가져온다.
		void Render();
	public:
		DirectionLight* GetDirectionLight() { return m_DirectionLight.get(); }
		DirectionLight* AddDirectionLight();
		PointLight* AddPointLight();
		SpotLight*  AddSpotLight();
		void        DeletePointLight(PointLight* light);
		void        DeleteSpotLight(SpotLight* light);
		void BindingCamera(Camera* cam);
	private:
		void InitHlsl(const DxWinConfig& config);
	};
}