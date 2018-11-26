#pragma once
#include"Common/JGRCCommon.h"
#include"Common/JGRCGlobalType.h"
#include"DirectXCommon.h"
#include"SRSRenderTarget.h"
#include"SRSScene.h"
namespace JGRC
{
	class CORE_EXPORT SRSShadowPass
	{
	private:
		class DirectX* m_Dx = nullptr;
		std::unique_ptr<SRSRenderTarget> m_RenderTarget;
		std::unique_ptr<SRSScene>        m_Scene;
		class LightPassLightManager*     m_LightPassLM = nullptr;
		SRSRenderTarget*                 m_pMainRenderTarget       = nullptr;
		class JGShaderArray*             m_DepthShader = nullptr;
	private:
		SRSShadowPass(const SRSShadowPass& copy) = delete;
		SRSShadowPass(SRSShadowPass&& srssp) = delete;
		SRSShadowPass& operator=(const SRSShadowPass& copy) = delete;
		SRSShadowPass& operator=(SRSShadowPass&& srssp) = delete;
	public:
		SRSShadowPass();
		~SRSShadowPass();
		void ShadowMapSetting(const real width, const real height, const real nearZ, const real farZ);
		void ShadowMapResolutionSetting(const real width, const real height);
		void ShadowMapDepthSetting(const real nearZ, const real farZ);
		void Init(const DxWinConfig& config, SRSRenderTarget* MainRT, class LightPassLightManager* LM);
		void Render(class Camera* cam, class JGViewport* viewport);

		ID3D11ShaderResourceView* GetShadowMap();
	private:
		void OutputHlsl();
		void ReadHlsl(const DxWinConfig& config);
	};
}