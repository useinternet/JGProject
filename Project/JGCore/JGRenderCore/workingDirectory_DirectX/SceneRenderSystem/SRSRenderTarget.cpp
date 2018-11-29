#include"SRSRenderTarget.h"
#include"DirectX/DirectX.h"
#include"BufferSystem/JGBuffer.h"
using namespace std;
using namespace JGRC;
using namespace Microsoft::WRL;
DirectX* SRSRenderTarget::m_Dx = nullptr;
SRSRenderTarget::SRSRenderTarget()
{
	m_Dx = DirectX::GetInstance();
}
SRSRenderTarget::~SRSRenderTarget()
{

}
bool SRSRenderTarget::CreateSRSRenderTarget(const int width, const int height)
{
	HRESULT result = S_OK;
	// ÅØ½ºÃÄ2D ¸¸µé±â
	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.Width = width;
	TextureDesc.Height = height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_UNKNOWN;
	TextureDesc.SampleDesc.Count   = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	TextureDesc.Format = depthStencilTextureFormat;
	result = m_Dx->GetDevice()->CreateTexture2D(&TextureDesc, nullptr, m_DepthStencilRT.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error,"JGRC::SRSRenderTarget", "Failed Create DepthStencilRT")
		return false;
	}

	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.Format = basicColorTextureFormat;
	result = m_Dx->GetDevice()->CreateTexture2D(&TextureDesc, nullptr, m_ColorSpecIntensityRT.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create ColorSpecIntensityRT")
			return false;
	}

	TextureDesc.Format = normalTextureFormat;
	result = m_Dx->GetDevice()->CreateTexture2D(&TextureDesc, nullptr, m_NormalRT.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create NormalRT")
			return false;
	}

	TextureDesc.Format = specPowTextureFormat;
	result = m_Dx->GetDevice()->CreateTexture2D(&TextureDesc, nullptr, m_SpecPowerRT.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create SpecPowerRT")
			return false;
	}


	// ±íÀÌ ½ºÅÙ½Ç ºä ¸ñ·Ï
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthSVDesc = {
		depthStencilRenderViewFormat,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		0
	};

	result = m_Dx->GetDevice()->CreateDepthStencilView(m_DepthStencilRT.Get(), &DepthSVDesc, m_DepthStencilDSV.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create DepthStencilDSV")
		return false;
	}
	DepthSVDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
	result = m_Dx->GetDevice()->CreateDepthStencilView(m_DepthStencilRT.Get(), &DepthSVDesc, m_DepthStencilReadOnlyDSV.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create DepthStencilReadOnlyDSV")
		return false;
	}
	// ·»´õ Å¸°Ù ºä ¸ñ·Ï
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {
		basicColorRenderViewFormat,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};

	result = m_Dx->GetDevice()->CreateRenderTargetView(m_ColorSpecIntensityRT.Get(), &RTVDesc, m_ColorSpecIntensityRTV.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create ColorSpecIntensityRTV")
		return false;
	}

	RTVDesc.Format = normalRenderViewFormat;
	result = m_Dx->GetDevice()->CreateRenderTargetView(m_NormalRT.Get(), &RTVDesc, m_NormalRTV.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create NormalRTV")
		return false;
	}

	RTVDesc.Format = specPowRenderViewFormat;
	result = m_Dx->GetDevice()->CreateRenderTargetView(m_SpecPowerRT.Get(), &RTVDesc, m_SpecPowerRTV.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create SpecPowerRTV")
		return false;
	}


	// ¸®¼Ò½º ºä
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc =
	{
		depthStencilResourceViewFormat,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0
	};
	SRVDesc.Texture2D.MipLevels = 1;
	result = m_Dx->GetDevice()->CreateShaderResourceView(m_DepthStencilRT.Get(), &SRVDesc, m_DepthStencilSRV.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create DepthStencilSRV")
		return false;
	}

	SRVDesc.Format = basicColorResourceViewFormat;
	result = m_Dx->GetDevice()->CreateShaderResourceView(m_ColorSpecIntensityRT.Get(), &SRVDesc, m_ColorSpecIntensitySRV.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create ColorSpecIntensitySRV")
		return false;
	}


	SRVDesc.Format = normalResourceViewFormat;
	result = m_Dx->GetDevice()->CreateShaderResourceView(m_NormalRT.Get(), &SRVDesc, m_NormalSRV.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create NormalSRV")
		return false;
	}


	SRVDesc.Format = specPowResourceViewFormat;
	result = m_Dx->GetDevice()->CreateShaderResourceView(m_SpecPowerRT.Get(), &SRVDesc, m_SpecPowerSRV.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create SpecPowerSRV")
		return false;
	}


	D3D11_DEPTH_STENCIL_DESC descDepth;
	descDepth.DepthEnable = true;
	descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepth.DepthFunc      = D3D11_COMPARISON_LESS;
	descDepth.StencilEnable    = true;
	descDepth.StencilReadMask  = 0xFF;
	descDepth.StencilWriteMask = 0xFF;
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = {
		D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, 
		D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
	descDepth.FrontFace = stencilMarkOp;
	descDepth.BackFace = stencilMarkOp;


	result = m_Dx->GetDevice()->CreateDepthStencilState(&descDepth, m_DepthStencilState.GetAddressOf());
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::SRSRenderTarget", "Failed Create DepthStencilState")
		return false;
	}
	return true;
}
void SRSRenderTarget::BindingRenderTarget()
{
	// Clear the depth stencil
	m_Dx->GetContext()->ClearDepthStencilView(m_DepthStencilDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

	// You only need to do this if your scene doesn't cover the whole visible area
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_Dx->GetContext()->ClearRenderTargetView(m_ColorSpecIntensityRTV.Get(), ClearColor);
	m_Dx->GetContext()->ClearRenderTargetView(m_NormalRTV.Get(), ClearColor);
	m_Dx->GetContext()->ClearRenderTargetView(m_SpecPowerRTV.Get(), ClearColor);

	// Bind all the render targets togther
	ID3D11RenderTargetView* rt[3] = { m_ColorSpecIntensityRTV.Get(), m_NormalRTV.Get(), m_SpecPowerRTV.Get() };
	m_Dx->GetContext()->OMSetRenderTargets(3, rt, m_DepthStencilDSV.Get());
	m_Dx->GetContext()->OMSetDepthStencilState(m_DepthStencilState.Get(), 1);
}
void SRSRenderTarget::ReleaseRenderTarget()
{
	m_Dx->GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Little cleanup
	ID3D11RenderTargetView* rt[3] = { nullptr, nullptr, nullptr };
	m_Dx->GetContext()->OMSetRenderTargets(3, rt, m_DepthStencilReadOnlyDSV.Get());
}

