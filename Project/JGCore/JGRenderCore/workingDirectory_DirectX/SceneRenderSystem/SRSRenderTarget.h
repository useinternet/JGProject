#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
namespace JGRC
{
	enum class ERTType
	{
		Position,
		Albedo,
		Normal,
		Depth
	};
	/* 필요 함수
	1. 다이렉트 클래스에 렌더 타겟 바인딩( 갯수만큼 )
	2. 뷰포트 바인딩
	3. 렌더 타깃 클리어
	*/
	class JGViewport;
	class SRSRenderTarget
	{
		typedef Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
		typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
		typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
		typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
		const int BufferCount = 4;
	private:

	private:
		static class DirectX* m_Dx;
		std::vector<ID3D11Texture2D*>          m_RTTexture;
		std::vector<ID3D11RenderTargetView*>   m_RTV;
		std::vector<ID3D11ShaderResourceView*> m_SRV;

		ID3D11Texture2D* m_DSVTexture;
		ID3D11DepthStencilView* m_DSV;

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
