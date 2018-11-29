#pragma once
#include"Common/JGRCCommon.h"
#include"Common/JGRCGlobalType.h"

namespace JGRC
{
	class CORE_EXPORT DirectionLightPass
	{
	private:
		std::string m_PassShaderPath = Game::path / "Engine/Shader/Shader/LightPass";
		std::string m_vsHlslPath     = Game::path / "Engine/Shader/HLSL/DirLightPass_vs.hlsl";
		std::string m_psHlslPath     = Game::path / "Engine/Shader/HLSL/DirLightPass_ps.hlsl";
		class DirectX* m_Dx = nullptr;
		class JGShaderArray* m_Shader = nullptr;
	private:
		DirectionLightPass(const DirectionLightPass& copy) = delete;
		DirectionLightPass(DirectionLightPass&& dlp)       = delete;
		DirectionLightPass& operator=(const DirectionLightPass& copy) = delete;
		DirectionLightPass& operator=(DirectionLightPass&& dlp)       = delete;
	public:
		DirectionLightPass();
		~DirectionLightPass();
		bool Init(const DxWinConfig& config, const bool isUpdate = true);
		void Render(class SRSRenderTarget* SRSRT, class Camera* cam, class JGViewport* viewport);
	};


}