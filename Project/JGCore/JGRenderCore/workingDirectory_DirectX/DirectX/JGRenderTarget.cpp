#include"JGRenderTarget.h"
using namespace JGRC;


JGRenderTarget::JGRenderTarget()
{
	m_D3DTexture2D.reserve(2);
	m_D3DTexture2D.resize(2);
}
JGRenderTarget::~JGRenderTarget()
{

}

bool JGRenderTarget::CreateRenderTarget(ID3D11Device* Device,IDXGISwapChain* SwapChain,
	const int Width, const int Height)
{
	m_Width = Width; m_Height = Height;

	HRESULT result = S_OK;
	D3D11_TEXTURE2D_DESC          BufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthViewDesc;


	// 백버퍼의 포인터를 가져옵니다.
	result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(void**)m_D3DTexture2D[static_cast<int>(ERenderTextureType::RenderTarget)].GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}
	// 백버퍼의 포인터로 렌더타겟 뷰를 생성합니다.
	result = Device->CreateRenderTargetView(m_D3DTexture2D[static_cast<int>(ERenderTextureType::RenderTarget)].Get(),
		nullptr, m_RenderTarget.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	// 깊이 버퍼 텍스쳐 생성
	WriteTextureDesc(BufferDesc, DXGI_FORMAT_D24_UNORM_S8_UINT,D3D11_BIND_DEPTH_STENCIL);
	result = Device->CreateTexture2D(&BufferDesc, nullptr,
		m_D3DTexture2D[static_cast<int>(ERenderTextureType::DepthBuffer)].GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	// 깊이-스텐실 뷰를 생성. 
	WriteDepthStencilViewDesc(DepthViewDesc);
	result = Device->CreateDepthStencilView(m_D3DTexture2D[static_cast<int>(ERenderTextureType::DepthBuffer)].Get(),
		&DepthViewDesc, m_DepthStencilView.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}
	return true;
}
ID3D11RenderTargetView* JGRenderTarget::Get()
{
	if (m_RenderTarget.Get())
	{
		return m_RenderTarget.Get();
	}
	return nullptr;
}
ID3D11RenderTargetView ** JGRenderTarget::GetAddress()
{
	return m_RenderTarget.GetAddressOf();
}
ID3D11Texture2D* JGRenderTarget::GetD3DTexture2D(const ERenderTextureType type)
{
	if (m_D3DTexture2D[static_cast<int>(type)].Get())
	{
		return m_D3DTexture2D[static_cast<int>(type)].Get();
	}
	return nullptr;
}
ID3D11DepthStencilView* JGRenderTarget::GetDepthStencilView()
{
	if (m_DepthStencilView.Get())
	{
		return m_DepthStencilView.Get();
	}
	return nullptr;
}
ID3D11DepthStencilView ** JGRenderTarget::GetDepthStencilViewAddress()
{
	return m_DepthStencilView.GetAddressOf();
}
void JGRenderTarget::WriteTextureDesc(D3D11_TEXTURE2D_DESC& Desc, const DXGI_FORMAT& format,
	UINT BindFlags, D3D11_USAGE Usage)
{
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Width = m_Width;
	Desc.Height = m_Height;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = format;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Usage = Usage;
	Desc.BindFlags = BindFlags;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
}

void JGRenderTarget::WriteDepthStencilViewDesc(D3D11_DEPTH_STENCIL_VIEW_DESC& Desc)
{
	// 깊이-스텐실 뷰의 서술자 작성
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	Desc.Texture2D.MipSlice = 0;
}
