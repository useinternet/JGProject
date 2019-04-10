#include<PCH.h>
#include"Scene.h"
#include"DirectXToolKit/DirectXCore.h"
#include"DirectXToolKit/CommandQueue.h"
#include"DirectXToolKit/CommandList.h"
#include"GBuffer.h"
#include"ShaderCommonDefines.h"
#include"DirectXToolKit/Shader.h"
using namespace Dx12;
using namespace std;

Scene::Scene(int width, int height)
{
	m_ChangedDebug = false;
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
void  Scene::DebugModeOn(int GbufferSlot)  
{
	if (m_ChangedDebug)
		return;
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
	m_ScreenPSO = DxDevice::GetShaderCommonDefines()->GetPSO(PreparedPSO::Screen, shader);
	m_ChangedDebug = true;
}
void  Scene::DebugModeOff() 
{
	GraphicsShader shader;
	shader.AddShaderPaths({ ShaderStage::Pixel, ShaderStage::Vertex }, SCENE_SHADER_PATH);
	shader.Compile();

	m_ScreenPSO = DxDevice::GetShaderCommonDefines()->GetPSO(PreparedPSO::Screen, shader);
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

	commandList->SetGraphicsRootSignature(DxDevice::GetShaderCommonDefines()->GetSceneRootSig());
	if (gbuffer)
	{
		commandList->SetGraphicsDescriptorTable(
			GBufferTexture::Albedo,
			gbuffer->GetTexture(GBufferTexture::Albedo).GetShaderResourceView());
		commandList->SetGraphicsDescriptorTable(
			GBufferTexture::Normal,
			gbuffer->GetTexture(GBufferTexture::Normal).GetShaderResourceView());
		commandList->SetGraphicsDescriptorTable(
			GBufferTexture::Specular,
			gbuffer->GetTexture(GBufferTexture::Specular).GetShaderResourceView());
		commandList->SetGraphicsDescriptorTable(
			GBufferTexture::Depth,
			gbuffer->GetTexture(GBufferTexture::Depth).GetShaderResourceView());
	}

	commandList->SetPipelineState(m_ScreenPSO);
	commandList->SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->Draw(6);
	m_ChangedDebug = false;
}
const  Texture& Scene::GetTexture()
{
	return m_RenderTarget.GetTexture(RtvSlot::Slot_0);
}