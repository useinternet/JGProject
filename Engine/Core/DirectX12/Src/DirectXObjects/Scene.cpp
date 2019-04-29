#include<PCH.h>
#include"Scene.h"
#include"DirectXToolKit/DirectXCore.h"
#include"DirectXToolKit/CommandQueue.h"
#include"DirectXToolKit/CommandList.h"
#include"GBuffer.h"
#include"ShaderCommonDefines.h"
#include"DirectXToolKit/Shader.h"
#include"DirectXToolKit/Camera.h"
using namespace Dx12;
using namespace std;

Scene::Scene(int width, int height)
{
	m_Viewport.Set((float)width, (float)height);
	m_ScissorRect.Set(width, height);

	DebugModeOff();


	D3D12_CLEAR_VALUE clearColor;
	clearColor.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	clearColor.Color[0] = 0.0f;
	clearColor.Color[1] = 0.0f;
	clearColor.Color[2] = 0.0f;
	clearColor.Color[3] = 1.0f;

	Texture texture(TextureUsage::RenderTarget,
		CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 
			width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		&clearColor,
		"SceneTexture");

	clearColor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearColor.DepthStencil.Depth = 1.0f;
	clearColor.DepthStencil.Stencil = 0;
	Texture depthstencil(TextureUsage::DepthStencil, 
		CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			&clearColor, "SceneDepthTexture");



	m_RenderTarget.AttachTexture(RtvSlot::Slot_0, texture);
	m_RenderTarget.AttachTexture(RtvSlot::DepthStencil, depthstencil);
	m_RenderTarget.SetRenderTargetClearColor(RtvSlot::Slot_0, { 0.0f,0.0f,0.0f,1.0f });
	m_RenderTarget.SetDepthStencilClearColor(1.0f, 0, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);
}
Scene::Scene(int width, int height, const RenderTarget& rendertarget) : m_RenderTarget(rendertarget)
{
	m_Viewport.Set((float)width, (float)height);
	m_ScissorRect.Set(width, height);

	DebugModeOff();
}
void  Scene::DebugModeOn(int GbufferSlot)  
{
	EGBufferTexture textureSlot = GbufferSlot;
	GraphicsShader shader;
	shader.AddShaderPaths({ ShaderStage::Pixel, ShaderStage::Vertex }, SCENE_SHADER_PATH);

	switch (textureSlot)
	{
	case GBufferTexture::Albedo:
		shader.AddMacro(ShaderStage::Pixel, SHADER_MACRO_SCENE_DEBUG_MODE_ALBEDO);
		break;
	case GBufferTexture::Normal:
		shader.AddMacro(ShaderStage::Pixel, SHADER_MACRO_SCENE_DEBUG_MODE_NORMAL);
		break;
	case GBufferTexture::Specular:
		shader.AddMacro(ShaderStage::Pixel, SHADER_MACRO_SCENE_DEBUG_MODE_SPECULAR);
		break;
	case GBufferTexture::Depth:
		shader.AddMacro(ShaderStage::Pixel, SHADER_MACRO_SCENE_DEBUG_MODE_DEPTH);
		break;
	default:
		return;
	}
	shader.Compile();
	m_ScenePSO = DxDevice::GetShaderCommonDefines()->GetPSO(PreparedPSO::Scene, shader);
}
void  Scene::DebugModeOff() 
{
	GraphicsShader shader;
	shader.AddShaderPaths({ ShaderStage::Pixel, ShaderStage::Vertex }, SCENE_SHADER_PATH);
	shader.Compile();

	m_ScenePSO = DxDevice::GetShaderCommonDefines()->GetPSO(PreparedPSO::Scene, shader);
}
void  Scene::ReSize(int width, int height)
{
	m_Viewport.Set((float)width, (float)height);
	m_ScissorRect.Set(width, height);
	m_RenderTarget.Resize(width, height);
}
void  Scene::Draw(CommandList* commandList, GBuffer* gbuffer)
{
	commandList->SetViewport(m_Viewport);
	commandList->SetScissorRect(m_ScissorRect);
	commandList->ClearRenderTarget(m_RenderTarget);
	commandList->SetRenderTarget(m_RenderTarget);

	commandList->SetGraphicsRootSignature(DxDevice::GetShaderCommonDefines()->GetMainRootSig());
	if (gbuffer)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle[] = {
			gbuffer->GetTexture(GBufferTexture::Albedo).GetShaderResourceView(),
			gbuffer->GetTexture(GBufferTexture::Normal).GetShaderResourceView(),
			gbuffer->GetTexture(GBufferTexture::Specular).GetShaderResourceView(),
			gbuffer->GetTexture(GBufferTexture::Depth).GetShaderResourceView()
		};

		commandList->SetGraphicsDescriptorTables(CommonRootParam::Texture,
			4, handle);

	}
	commandList->SetPipelineState(m_ScenePSO);
	commandList->SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->Draw(6);
}
const  Texture& Scene::GetTexture() const
{
	return m_RenderTarget.GetTexture(RtvSlot::Slot_0);
}
const RenderTarget& Scene::GetRenderTarget() const {
	return m_RenderTarget;
}
RenderTarget& Scene::GetRenderTarget() {
	return m_RenderTarget;
}