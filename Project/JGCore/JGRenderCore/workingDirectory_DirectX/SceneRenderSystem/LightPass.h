#pragma once
#include"Common/JGRCCommon.h"
#include"Common/JGRCGlobalType.h"
// 다이렉트 라이트 씬 그리고
// 
namespace JGRC
{
	class DirectionLightPass;
	// depth 셰이더 따로 관리
	// 블렌딩 모드 따로 관리
	class CORE_EXPORT LightPass
	{
	private:
		class DirectX* m_Dx;
		class JGShaderArray* m_Shader;
		std::shared_ptr<DirectionLightPass> m_DirLightPass;
	public:
		void InitPass(const DxWinConfig& config);
		void Draw(class SRSRenderTarget* SRSRT, class Camera* cam, class JGViewport* viewport);
	};
}
