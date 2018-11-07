#include"SRSRenderTarget.h"
#include"DirectX/JGViewport.h"
#include"DirectX/DirectX.h"
using namespace std;
using namespace JGRC;
using namespace Microsoft::WRL;
DirectX* SRSRenderTarget::m_Dx = nullptr;
SRSRenderTarget::SRSRenderTarget()
{
	m_Dx = DirectX::GetInstance();
	m_viewPort = make_unique<JGViewport>();
}
SRSRenderTarget::~SRSRenderTarget()
{
	for (auto& iter : m_SRV)
	{
		iter->Release();
		iter = nullptr;
	}
	for (auto& iter : m_RTTexture)
	{
		iter->Release();
		iter = nullptr;
	}
	for (auto& iter : m_RTV)
	{
		iter->Release();
		iter = nullptr;
	}
	m_DSVTexture->Release();
	m_DSVTexture = nullptr;
	m_DSV->Release();
	m_DSV = nullptr;
}

bool SRSRenderTarget::CreateSRSRenderTarget(const int width, const int height)
{
	HRESULT result = S_OK;
	// ·»´õ Å¸°Ù ÅØ½ºÃÄ ¸ñ·Ï ÀÛ¼º
	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(TextureDesc));
	TextureDesc.Width  = (UINT)width;
	TextureDesc.Height = (UINT)height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;


	// ·»´õ Å¸°Ù ÅØ½ºÃÄ »ý¼º
	for (uint i = 0; i < BufferCount; ++i)
	{
		ID3D11Texture2D* RttTex;
		result = m_Dx->GetDevice()->CreateTexture2D(
			&TextureDesc, nullptr, &RttTex);
		if (FAILED(result))
		{
			JGLOG(log_Critical, "JGRC::SRSRenderTarget", "Failed Create RenderTargetTexture");
			return false;
		}
		m_RTTexture.push_back(move(RttTex));
	}

	// ·»´õ Å¸°Ù ºä ¸ñ·Ï ÀÛ¼º
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(RTVDesc));
	RTVDesc.Format = TextureDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;
	
	// ·»´õ Å¸°Ù ºä »ý¼º
	for (uint i = 0; i < BufferCount; ++i)
	{
		ID3D11RenderTargetView* RTV;
		result = m_Dx->GetDevice()->CreateRenderTargetView(
			m_RTTexture[i], &RTVDesc, &RTV);
		if (FAILED(result))
		{
			JGLOG(log_Critical, "JGRC::SRSRenderTarget", "Failed Create RenderTarget");
			return false;
		}
		m_RTV.push_back(move(RTV));
	}
	// ¼ÎÀÌ´õ ¸®¼Ò½ººä ¸ñ·Ï ÀÛ¼º
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = TextureDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	// ¼ÎÀÌ´õ ¸®¼Ò½ººä »ý¼º
	for (uint i = 0; i < BufferCount; ++i)
	{
		ID3D11ShaderResourceView* SRV;
		result = m_Dx->GetDevice()->CreateShaderResourceView(
			m_RTTexture[i], &SRVDesc, &SRV);
		if (FAILED(result))
		{
			JGLOG(log_Critical, "JGRC::SRSRenderTarget", "Failed Create ShaderResourceView")
			return false;
		}
		m_SRV.push_back(move(SRV));
	}
	// ±íÀÌ ¹öÆÛ ÅØ½ºÃÄ »ý¼º
	D3D11_TEXTURE2D_DESC DSVTextureDesc;
	ZeroMemory(&DSVTextureDesc, sizeof(DSVTextureDesc));


	DSVTextureDesc.Width = (UINT)width;
	DSVTextureDesc.Height = (UINT)height;
	DSVTextureDesc.MipLevels = 1;
	DSVTextureDesc.ArraySize = 1;
	DSVTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVTextureDesc.SampleDesc.Count = 1;
	DSVTextureDesc.SampleDesc.Quality = 0;
	DSVTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	DSVTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DSVTextureDesc.CPUAccessFlags = 0;
	DSVTextureDesc.MiscFlags = 0;

	result = m_Dx->GetDevice()->CreateTexture2D(&DSVTextureDesc, nullptr, &m_DSVTexture);
	if (FAILED(result))
	{
		JGLOG(log_Critical, "JGRC::SRSRenderTarget", "Failed Create DepthTexture")
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	ZeroMemory(&DSVDesc, sizeof(DSVDesc));
	DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Texture2D.MipSlice = 0;

	result = m_Dx->GetDevice()->CreateDepthStencilView(m_DSVTexture, &DSVDesc, &m_DSV);
	if (FAILED(result))
	{
		JGLOG(log_Critical, "JGRC::SRSRenderTarget", "Failed Create DepthStencilView")
		return false;
	}
	m_viewPort->InitViewport(width, height, 0, 0, 0);
	return true;
}
void SRSRenderTarget::BindingRenderTarget()
{
	vector<ID3D11RenderTargetView*> vRTVarr;
	for (uint i = 0; i < BufferCount; ++i)
	{
		vRTVarr.push_back(m_RTV[i]);
	}
	m_Dx->GetContext()->OMSetRenderTargets(BufferCount, &vRTVarr[0], m_DSV);
	m_Dx->GetContext()->RSSetViewports(1, m_viewPort->Get());
}
void SRSRenderTarget::ClearRenderTarget(const real r, const real g, const real b, const real a)
{
	real color[4] = { r,g,b,a };
	for (uint i = 0; i < BufferCount; ++i)
	{
		m_Dx->GetContext()->ClearRenderTargetView(m_RTV[i], color);
	}
	m_Dx->GetContext()->ClearDepthStencilView(m_DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
ID3D11ShaderResourceView* SRSRenderTarget::GetShaderResourceView(const ERTType type)
{
	return m_SRV[(int)type];
}
