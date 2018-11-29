#include"LightPass.h"
#include"DirectX/DirectX.h"
#include"LightPass/DirectionLightPass.h"
#include"MaterialSystem/Shader/HlslEditor.h"
#include"MaterialSystem/Shader/JGShaderArray.h"
#include"MaterialSystem/Shader/ShaderReader.h"
#include"MaterialSystem/Shader/ShaderWriter.h"
#include"SceneRenderSystem/SRSRenderTarget.h"
#include"Camera/Camera.h"
#include"DirectX/JGViewport.h"

// 현재 상수 버퍼 
using namespace JGRC;
using namespace std;
void LightPass::InitPass(const DxWinConfig& config)
{
	m_Dx = DirectX::GetInstance();
	m_DirLightPass = make_unique<DirectionLightPass>();
	m_DirLightPass->Init(config);
}
void LightPass::Draw(SRSRenderTarget* SRSRT, Camera* cam, JGViewport* viewport)
{
	m_Dx->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOff);
	m_DirLightPass->Render(SRSRT, cam, viewport);
	m_Dx->SetDirectState(EStateType::DepthState, (uint)EDepthStateType::ZBufferOn);
}
