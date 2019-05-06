#include<PCH.h>
#include"GBuffer.h"
#include"Dx12Object.h"
#include"DirectXToolKit/DirectXCore.h"
#include"DirectXToolKit/CommandQueue.h"
#include"DirectXToolKit/CommandList.h"
#include"Material.h"
#include"ShaderCommonDefines.h"
using namespace std;
using namespace Dx12;

std::vector<DXGI_FORMAT> GBuffer::ms_RtvFormatArray;

const std::vector<DXGI_FORMAT>& GBuffer::GetRtvFormatArray()
{
	if (ms_RtvFormatArray.empty())
	{
		ms_RtvFormatArray = { ms_AlebdoFormat , ms_NormalFormat,ms_SpecularFormat, ms_DepthFormat };
	}
	return ms_RtvFormatArray;
}
GBuffer::GBuffer(int width, int height)
{
	m_Viewport.Set((float)width, (float)height);
	m_ScissorRect.Set(width, height);

	D3D12_RESOURCE_DESC resourceDesc = {};
	D3D12_CLEAR_VALUE clearValue;

	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 0.0f;

	// Albedo Desc
	clearValue.Format = ms_AlebdoFormat;
	resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(ms_AlebdoFormat, width, height,
		1,0,1,0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	Texture albedo(TextureUsage::RenderTarget, resourceDesc, &clearValue, "GBuffer_Alebdo_Texture");

	clearValue.Format = ms_NormalFormat;
	resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(ms_NormalFormat, width, height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	Texture normal(TextureUsage::RenderTarget, resourceDesc, &clearValue, "GBuffer_Normal_Texture");

	clearValue.Format = ms_SpecularFormat;
	resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(ms_SpecularFormat, width, height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	Texture specular(TextureUsage::RenderTarget, resourceDesc, &clearValue, "GBuffer_Specular_Texture");


	clearValue.Format = ms_DepthFormat;
	resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(ms_DepthFormat, width, height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	clearValue.Color[0] = 1.0f;
	Texture depth(TextureUsage::RenderTarget, resourceDesc, &clearValue, "GBuffer_Depth_Texture");


	

	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;
	resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	Texture depthstencil(TextureUsage::RenderTarget, resourceDesc, &clearValue, "GBuffer_DepthStencil_Texture");

	m_RenderTarget.AttachTexture(RtvSlot::Slot_0, albedo);
	m_RenderTarget.AttachTexture(RtvSlot::Slot_1, normal);
	m_RenderTarget.AttachTexture(RtvSlot::Slot_2, specular);
	m_RenderTarget.AttachTexture(RtvSlot::Slot_3, depth);
	m_RenderTarget.AttachTexture(RtvSlot::DepthStencil, depthstencil);


	m_RenderTarget.SetRenderTargetClearColor(RtvSlot::Slot_0, { 0.0f,0.0f,0.0f,0.0f });
	m_RenderTarget.SetRenderTargetClearColor(RtvSlot::Slot_1, { 0.0f,0.0f,0.0f,0.0f });
	m_RenderTarget.SetRenderTargetClearColor(RtvSlot::Slot_2, { 0.0f,0.0f,0.0f,0.0f });
	m_RenderTarget.SetRenderTargetClearColor(RtvSlot::Slot_3, { 1.0f,0.0f,0.0f,0.0f });
	m_RenderTarget.SetDepthStencilClearColor(1.0f, 0, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);
}


void GBuffer::Resize(int width, int height)
{
	m_Viewport.Set((float)width, (float)height);
	m_ScissorRect.Set(width, height);
	m_RenderTarget.Resize(width, height);
}

void GBuffer::Draw(CommandList* commandList,
	const std::vector<Dx12Object*>& objArray,
	const PassCB& passcb)
{
	commandList->SetViewport(m_Viewport);
	commandList->SetScissorRect(m_ScissorRect);

	commandList->ClearRenderTarget(m_RenderTarget);
	commandList->SetRenderTarget(m_RenderTarget);

	commandList->SetGraphicsRootSignature(DxDevice::GetShaderCommonDefines()->GetMainRootSig());
	commandList->SetGraphicsConstantBufferView(CommonRootParam::PassCB, &passcb);
	commandList->SetGraphicsShaderResourceView(CommonRootParam::MaterialCB, Material::GetMaterialCBArray());

	for (auto& obj : objArray)
	{
		obj->Draw(commandList);
	}
}

const Texture& GBuffer::GetTexture(EGBufferTexture gt)
{
	return m_RenderTarget.GetTexture(gt);
}