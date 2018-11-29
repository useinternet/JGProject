#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
namespace JGRC
{
	class CORE_EXPORT SRSRenderTarget
	{
	private:
#pragma pack(push,1)
		struct CB_GBUFFER_UNPACK
		{
			jgVec4       PerspectiveValues;
			jgMatrix4x4  ViewInv;
		};
#pragma pack(pop)
	private:
		typedef Microsoft::WRL::ComPtr<ID3D11Texture2D>          Texture2D;
		typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   RenderTargetView;
		typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
		typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   DepthStencilView;
		typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  DepthStencilState;
	private:
		static class DirectX* m_Dx;
		// Texture formats
		static const DXGI_FORMAT depthStencilTextureFormat = DXGI_FORMAT_R24G8_TYPELESS;
		static const DXGI_FORMAT basicColorTextureFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT normalTextureFormat       = DXGI_FORMAT_R11G11B10_FLOAT;
		static const DXGI_FORMAT specPowTextureFormat      = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Render view formats
		static const DXGI_FORMAT depthStencilRenderViewFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		static const DXGI_FORMAT basicColorRenderViewFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT normalRenderViewFormat       = DXGI_FORMAT_R11G11B10_FLOAT;
		static const DXGI_FORMAT specPowRenderViewFormat      = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Resource view formats
		static const DXGI_FORMAT depthStencilResourceViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		static const DXGI_FORMAT basicColorResourceViewFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT normalResourceViewFormat  = DXGI_FORMAT_R11G11B10_FLOAT;
		static const DXGI_FORMAT specPowResourceViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;


		Texture2D m_DepthStencilRT;
		Texture2D m_ColorSpecIntensityRT;
		Texture2D m_NormalRT;
		Texture2D m_SpecPowerRT;

		DepthStencilView m_DepthStencilDSV;
		DepthStencilView m_DepthStencilReadOnlyDSV;

		RenderTargetView m_ColorSpecIntensityRTV;
		RenderTargetView m_NormalRTV;
		RenderTargetView m_SpecPowerRTV;

		ShaderResourceView m_DepthStencilSRV;
		ShaderResourceView m_ColorSpecIntensitySRV;
		ShaderResourceView m_NormalSRV;
		ShaderResourceView m_SpecPowerSRV;

		DepthStencilState m_DepthStencilState;
	private:
		SRSRenderTarget(const SRSRenderTarget& copy)            = delete;
		SRSRenderTarget& operator=(const SRSRenderTarget& copy) = delete;
		SRSRenderTarget(SRSRenderTarget&& srsrt)                = delete;
		SRSRenderTarget& operator=(SRSRenderTarget&& srsrt)     = delete;
	public:
		SRSRenderTarget();
		~SRSRenderTarget();

		bool CreateSRSRenderTarget(const int width, const int height);
		void BindingRenderTarget();
		void ReleaseRenderTarget();

		ID3D11ShaderResourceView* GetDepthTexture()        { return m_DepthStencilSRV.Get(); }
		ID3D11ShaderResourceView* GetColorSpecInsTexture() { return m_ColorSpecIntensitySRV.Get(); }
		ID3D11ShaderResourceView* GetNormalTexture()       { return m_NormalSRV.Get(); }
		ID3D11ShaderResourceView* GetSpecPowerTexture()    { return m_SpecPowerSRV.Get(); }
	};
}
