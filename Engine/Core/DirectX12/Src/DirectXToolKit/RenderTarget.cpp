#include<PCH.h>
#include"RenderTarget.h"

using namespace Dx12;
using namespace std;

RenderTarget::RenderTarget()
{
	m_Textures.resize(RtvSlot::NumRtvSlot);
	m_RtvDescs.resize(RtvSlot::DepthStencil);
	m_RtvClearColors.resize(RtvSlot::DepthStencil);
}


RenderTarget::RenderTarget(const RenderTarget& copy)
{
	m_RtvDescs.resize(RtvSlot::DepthStencil);
	m_Textures = copy.m_Textures;
	for (int i = 0; i < RtvSlot::DepthStencil; ++i)
	{
		if(copy.m_RtvDescs[i])
			m_RtvDescs[i] = make_unique<D3D12_RENDER_TARGET_VIEW_DESC>(*copy.m_RtvDescs[i]);
	
	}
		
	if (copy.m_DsvDesc)
	{
		m_DsvDesc = make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>(*copy.m_DsvDesc);
	}
	m_RtvClearColors = copy.m_RtvClearColors;
	m_DsvClearDepth = copy.m_DsvClearDepth;
	m_DsvClearStencil = copy.m_DsvClearStencil;
	m_DsvClearFlag = copy.m_DsvClearFlag;
}
RenderTarget::RenderTarget(RenderTarget&& rhs)
{
	m_Textures = move(rhs.m_Textures);
	m_RtvDescs = move(rhs.m_RtvDescs);
	m_DsvDesc = move(rhs.m_DsvDesc);
	m_RtvClearColors = move(rhs.m_RtvClearColors);
	m_DsvClearDepth = rhs.m_DsvClearDepth;
	m_DsvClearStencil = rhs.m_DsvClearStencil;
	m_DsvClearFlag = rhs.m_DsvClearFlag;

	rhs.m_DsvClearDepth = 1.0f;
	rhs.m_DsvClearStencil = 0;
	rhs.m_DsvClearFlag = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;

	rhs.m_Textures.resize(RtvSlot::NumRtvSlot);
	rhs.m_RtvDescs.resize(RtvSlot::DepthStencil);
	rhs.m_RtvClearColors.resize(RtvSlot::DepthStencil);
}

RenderTarget& RenderTarget::operator=(const RenderTarget& rhs)
{
	m_Textures = rhs.m_Textures;
	for (int i = 0; i < RtvSlot::DepthStencil; ++i)
	{
		if (rhs.m_RtvDescs[i])
			m_RtvDescs[i] = make_unique<D3D12_RENDER_TARGET_VIEW_DESC>(*rhs.m_RtvDescs[i]);

	}
	if (rhs.m_DsvDesc)
	{
		m_DsvDesc = make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>(*rhs.m_DsvDesc);
	}
	m_RtvClearColors = rhs.m_RtvClearColors;
	m_DsvClearDepth = rhs.m_DsvClearDepth;
	m_DsvClearStencil = rhs.m_DsvClearStencil;
	m_DsvClearFlag = rhs.m_DsvClearFlag;
	return *this;
}
RenderTarget& RenderTarget::operator=(RenderTarget&& rhs)
{
	m_Textures = move(rhs.m_Textures);
	m_RtvDescs = move(rhs.m_RtvDescs);
	m_DsvDesc = move(rhs.m_DsvDesc);
	m_RtvClearColors = move(rhs.m_RtvClearColors);
	m_DsvClearDepth = rhs.m_DsvClearDepth;
	m_DsvClearStencil = rhs.m_DsvClearStencil;
	m_DsvClearFlag = rhs.m_DsvClearFlag;

	rhs.m_DsvClearDepth = 1.0f;
	rhs.m_DsvClearStencil = 0;
	rhs.m_DsvClearFlag = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;

	rhs.m_Textures.resize(RtvSlot::NumRtvSlot);
	rhs.m_RtvDescs.resize(RtvSlot::DepthStencil);
	rhs.m_RtvClearColors.resize(RtvSlot::DepthStencil);
	return *this;
}

void RenderTarget::AttachTexture(ERtvSlot slot, const Texture& texture)
{
	
	if (texture.GetDesc().Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
		texture.GetDesc().Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		m_Textures[slot] = texture;
	}
	else
	{
		DX12_LOG_ERROR("%s is not support rendertargetview", texture.GetName().c_str());
	}
}
void RenderTarget::SetRenderTargetDesc(ERtvSlot slot, D3D12_RENDER_TARGET_VIEW_DESC* desc)
{
	if (slot < RtvSlot::DepthStencil)
	{
		m_RtvDescs[slot] = make_unique<D3D12_RENDER_TARGET_VIEW_DESC>(*desc);
	}
	else
	{
		DX12_LOG_WARNING("rendertarget's %d slot is not exist", slot);
	}
}
void RenderTarget::SetDepthStencilDesc(D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
{
	m_DsvDesc = make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>(*desc);
}
void RenderTarget::SetRenderTargetClearColor(ERtvSlot slot, const DirectX::XMFLOAT4& color)
{
	if (slot < RtvSlot::DepthStencil)
	{
		m_RtvClearColors[slot] = color;
	}
	else
	{
		DX12_LOG_WARNING("rendertarget's %d slot is not exist", slot);
	}
}
void RenderTarget::SetDepthStencilClearColor(float depth, uint8_t stencil, D3D12_CLEAR_FLAGS flag)
{
	m_DsvClearDepth   = depth;
	m_DsvClearStencil = stencil;
	m_DsvClearFlag = flag;
}
void RenderTarget::Resize(uint32_t width, uint32_t height)
{
	for (auto& texture : m_Textures)
	{
		if (texture.IsValid())
		{
			texture.Resize(width, height);
		}
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetRenderTargetView(ERtvSlot slot) const
{
	if (slot < RtvSlot::DepthStencil)
	{
		return m_Textures[slot].GetRenderTargetView(m_RtvDescs[slot].get());
	}
	else
	{
		DX12_LOG_WARNING("rendertarget's %d slot is not exist", slot);
		return { 0 };
	}
}
D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetDepthStencilView() const
{
	return m_Textures[RtvSlot::DepthStencil].GetDepthStencilView(m_DsvDesc.get());
}
const DirectX::XMFLOAT4& RenderTarget::GetRenderTargetClearColor(ERtvSlot slot) const
{
	if (slot < RtvSlot::DepthStencil)
	{
		return m_RtvClearColors[slot];
	}
	else
	{
		DX12_LOG_WARNING("rendertarget's %d slot is not exist", slot);
		return m_RtvClearColors[0];
	}
}
float RenderTarget::GetDepthStencilClearDepth() const
{
	return m_DsvClearDepth;
}
uint8_t RenderTarget::GetDepthStencilClearStencil() const
{
	return m_DsvClearStencil;
}
D3D12_CLEAR_FLAGS RenderTarget::GetDepthStencilClearFlags() const
{
	return m_DsvClearFlag;
}
const Texture& RenderTarget::GetTexture(ERtvSlot slot) const
{
	return m_Textures[slot];
}
DXGI_FORMAT RenderTarget::GetTextureFormat(ERtvSlot slot) const
{
	return m_Textures[slot].GetDesc().Format;
}