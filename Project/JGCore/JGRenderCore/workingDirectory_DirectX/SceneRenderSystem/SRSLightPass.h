#pragma once
#include"SRSRenderTarget.h"
#include"SRSScene.h"
#include"LightPassLightManager.h"
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
		HWND m_hWnd;
		class DirectX* m_Dx;
		class JGShaderArray*  m_Shader;
		// 씬
		std::unique_ptr<SRSScene>        m_Scene;
		SRSRenderTarget* m_RenderTarget = nullptr;
		//
		std::unique_ptr<LightPassLightManager> m_LightManager;
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
		DirectionLight* GetDirectionLight();
		DirectionLight* AddDirectionLight();
		PointLight* AddPointLight();
		SpotLight*  AddSpotLight();
		void        DeletePointLight(PointLight* light);
		void        DeleteSpotLight(SpotLight* light);
		void BindingCamera(Camera* cam);
		LightPassLightManager* GetLightManager() { return m_LightManager.get(); }
		Camera* GetCamera()                      { return m_Camera; }
	private:
		virtual bool OutputHlsl();
		// 데이터 뽑는거와 데이터 불러오는거 분리
		void InitHlsl(const DxWinConfig& config);
	};
}