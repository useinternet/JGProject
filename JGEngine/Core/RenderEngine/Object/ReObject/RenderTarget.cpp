#include "pch.h"
#include "RenderTarget.h"

#define MAX_RENDER_TARGET 8
using namespace std;

namespace RE
{
	uint32_t RenderTarget::GetMaxNumRenderTarget()
	{
		return MAX_RENDER_TARGET;
	}
	RenderTarget::RenderTarget() : ReObject("RenderTarget")
	{
		m_Textures.resize(MAX_RENDER_TARGET);
	}


	void RenderTarget::BindTexture(uint32_t slot, const Texture& texture)
	{
		if (slot >= MAX_RENDER_TARGET)
		{
			RE_LOG_WARN("You have exceeded the maximum slot of{ 0 }.", GetName());
			return;
		}
		if ((texture.GetDesc().Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) == 0)
		{
			RE_LOG_WARN("{0} cannot be render target. Please change resource flag", texture.GetName());
		}





		m_Textures[slot] = texture;
	}
	void RenderTarget::BindDepthTexture(
		const Texture& texture, D3D12_CLEAR_FLAGS flags)
	{
		if ((texture.GetDesc().Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) == 0)
		{
			RE_LOG_WARN("{0} cannot be dephstencil target. Please change resource flag", texture.GetName());
		}
		m_DepthTexture = texture;
		m_ClearFlags = flags;
	}

	const Texture& RenderTarget::GetTexture(uint32_t slot) const
	{
		if (slot >= MAX_RENDER_TARGET)
		{
			RE_LOG_WARN("You have exceeded the maximum slot of{ 0 }", GetName());
			return m_Textures[0];
		}
		return m_Textures[slot];
	}
	const Texture& RenderTarget::GetDepthTexture() const
	{
		return m_DepthTexture;
	}
	Texture* RenderTarget::GetTexture(uint32_t slot)
	{
		if (slot >= MAX_RENDER_TARGET)
		{
			RE_LOG_WARN("You have exceeded the maximum slot of{ 0 }", GetName());
			return &m_Textures[0];
		}
		return &m_Textures[slot];
	}
	Texture* RenderTarget::GetDepthTexture()
	{
		return &m_DepthTexture;
	}
	const Color& RenderTarget::GetClearColor(uint32_t slot) const
	{
		if (slot >= MAX_RENDER_TARGET)
		{
			RE_LOG_WARN("You have exceeded the maximum slot of{ 0 } ", GetName());
			return m_Textures[0].GetClearColor();
		}
		return m_Textures[slot].GetClearColor();
	}
	float RenderTarget::GetClearDepth() const
	{
		return m_DepthTexture.GetClearDepth();
	}
	uint8_t RenderTarget::GetClearStencil() const
	{
		return m_DepthTexture.GetClearStencil();
	}
	D3D12_CLEAR_FLAGS RenderTarget::GetDepthClearFlags() const
	{
		return m_ClearFlags;
	}
	void RenderTarget::Resize(uint32_t width, uint32_t height)
	{
		for (auto& texture : m_Textures)
		{
			texture.Resize(width, height);
		}
		m_DepthTexture.Resize(width, height);
	}
}