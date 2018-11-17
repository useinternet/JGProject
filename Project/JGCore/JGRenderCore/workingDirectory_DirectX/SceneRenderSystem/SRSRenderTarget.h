#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
namespace JGRC
{
	enum class CORE_EXPORT ERTType
	{
		Pos_Depth,
		Normal_SpecPw,
		Albedo_SpecIts,
		SpecColor_pad
	};
	class JGViewport;
	class CORE_EXPORT SRSRenderTarget
	{
		typedef Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
		typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
		typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
		typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
		const int BufferCount = 4;
	private:

	private:
		static class DirectX* m_Dx;
		std::vector<Texture2D>          m_RTTexture;
		std::vector<RenderTargetView>   m_RTV;
		std::vector<ShaderResourceView> m_SRV;

		Texture2D m_DSVTexture;
		DepthStencilView m_DSV;

		int m_TextureWidth;
		int m_TextureHeight;
		std::unique_ptr<JGViewport> m_viewPort;
	private:
		SRSRenderTarget(const SRSRenderTarget& copy) = delete;
		SRSRenderTarget& operator=(const SRSRenderTarget& copy) = delete;
	public:
		SRSRenderTarget();
		~SRSRenderTarget();

		bool CreateSRSRenderTarget(const int width, const int height);
		void BindingRenderTarget();
		void ClearRenderTarget(const real r = 0, const real g = 0 , const real b = 0, const real a = 0);
		ID3D11ShaderResourceView* GetShaderResourceView(const ERTType type);
	};
}
