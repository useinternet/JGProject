#pragma once
#include "RE.h"
#include "Object/ReObject.h"
#include "Object/DxObject/Resource.h"
#include <vector>
namespace RE
{
	class RenderTarget : public ReObject
	{
	public:
		static uint32_t GetMaxNumRenderTarget();
	public:
		RenderTarget();
		void BindTexture(uint32_t slot, const Texture& texture);
		void BindDepthTexture(
			const Texture& texture,
			D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);
		const Texture& GetTexture(uint32_t slot) const;
		const Texture& GetDepthTexture() const;
		Texture* GetTexture(uint32_t slot);
		Texture* GetDepthTexture();
		const JColor& GetClearColor(uint32_t slot) const;
		float GetClearDepth() const;
		uint8_t GetClearStencil() const;
		D3D12_CLEAR_FLAGS GetDepthClearFlags() const;
		void Resize(uint32_t width, uint32_t height);


		void Clone(RenderTarget& rt);
	private:
		std::vector<Texture> m_Textures;
		Texture m_DepthTexture;
		D3D12_CLEAR_FLAGS m_ClearFlags;
	};
}