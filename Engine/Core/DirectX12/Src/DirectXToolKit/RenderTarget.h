#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"Texture.h"
namespace Dx12
{
	typedef int ERtvSlot;
	namespace RtvSlot
	{
		enum
		{
			Slot_0 = 0,
			Slot_1,
			Slot_2,
			Slot_3,
			Slot_4,
			Slot_5,
			Slot_6,
			Slot_7,
			DepthStencil,
			NumRtvSlot
		};
	}
	class RenderTarget
	{
	private:
		std::vector<Texture> m_Textures;
		// desc
		std::vector<std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC>> m_RtvDescs;
		std::unique_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC> m_DsvDesc;
		// clearcolor
		std::vector<DirectX::XMFLOAT4> m_RtvClearColors;
		float   m_DsvClearDepth   = 1.0f;
		uint8_t m_DsvClearStencil = 0;
		D3D12_CLEAR_FLAGS m_DsvClearFlag = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
	public:
		RenderTarget();
		~RenderTarget() = default;
	public:
		RenderTarget(const RenderTarget& copy);
		RenderTarget(RenderTarget&& rhs);

		RenderTarget& operator=(const RenderTarget& rhs);
		RenderTarget& operator=(RenderTarget&& rhs);
	public:
		void AttachTexture(ERtvSlot slot, const Texture& texture);
		void SetRenderTargetDesc(ERtvSlot slot, D3D12_RENDER_TARGET_VIEW_DESC* desc);
		void SetDepthStencilDesc(D3D12_DEPTH_STENCIL_VIEW_DESC* desc);
		void SetRenderTargetClearColor(ERtvSlot slot, const DirectX::XMFLOAT4& color);
		void SetDepthStencilClearColor(float depth, uint8_t stencil, D3D12_CLEAR_FLAGS flag);
		void Resize(uint32_t width, uint32_t height);
	public:
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView(ERtvSlot slot) const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
		const DirectX::XMFLOAT4& GetRenderTargetClearColor(ERtvSlot slot) const;
		float GetDepthStencilClearDepth() const;
		uint8_t GetDepthStencilClearStencil() const;
		D3D12_CLEAR_FLAGS GetDepthStencilClearFlags() const;
		const Texture& GetTexture(ERtvSlot slot) const;
		DXGI_FORMAT GetTextureFormat(ERtvSlot slot) const;
	};
}