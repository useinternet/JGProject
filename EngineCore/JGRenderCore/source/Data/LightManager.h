#pragma once
#include"DxCommon/DxCommon.h"
#include"JGLight.h"

namespace JGRC
{
	static const std::wstring Shadow_hlsl_file_path = L"../Contents/Engine/Shaders/Shadows.hlsl";
	class RCORE_EXPORT LightManager
	{
	private:
		LightManager(const LightManager& rhs)            = delete;
		LightManager& operator=(const LightManager& rhs) = delete;
	private:
		UINT m_DirectionLightCount = 0;
		UINT m_PointLightCount     = 0;
		UINT m_SpotLightCount      = 0;
		UINT m_LightCount = 0;
		std::unique_ptr<JGLight>              m_DirLight;
		std::vector<std::unique_ptr<JGLight>> m_PointLights;
		std::vector<std::unique_ptr<JGLight>> m_SpotLights;
		// 그림자 셰이더
		std::unique_ptr<class Shader> m_ShadowShader;
		ID3D12PipelineState* m_ShadowPSO;
	public:
		LightManager()  = default;
		~LightManager() = default;
		void BuildLight();
		void BuildShadowShader(class CommonShaderRootSignature* CommonSig);
		void Update(FrameResource* CurrFrameResource);
		void DrawShadowMap(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource);

		JGLight* AddLight(ELightType type);
		JGLight* DeleteLight(JGLight* light);
		UINT Size() const;
	};
}