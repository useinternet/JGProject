#include"JGRenderTargetD.h"
#include"../EngineStatics/JGLog.h"



JGRenderTargetD::JGRenderTargetD()
{
	m_D3DTexture2D.reserve(2);
	m_D3DTexture2D.resize(2);
}
JGRenderTargetD::~JGRenderTargetD()
{

}

bool JGRenderTargetD::CreateRenderTarget(ID3D11Device* Device,IDXGISwapChain* SwapChain,
	const int Width, const int Height)
{
	m_Width = Width; m_Height = Height;

	HRESULT result = S_OK;
	D3D11_TEXTURE2D_DESC          BufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthViewDesc;


	// 백버퍼의 포인터를 가져옵니다.
	result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(void**)m_D3DTexture2D[EnumClassToInt(ERenderTextureType::RenderTarget)].GetAddressOf());
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Load Texture2D of SwapChain Failed."));
		return false;
	}
	// 백버퍼의 포인터로 렌더타겟 뷰를 생성합니다.
	result = Device->CreateRenderTargetView(m_D3DTexture2D[EnumClassToInt(ERenderTextureType::RenderTarget)].Get(), 
		nullptr, m_RenderTarget.GetAddressOf());
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create RenderTargetView Failed."));
		return false;
	}

	// 깊이 버퍼 텍스쳐 생성
	WriteTextureDesc(BufferDesc, DXGI_FORMAT_D24_UNORM_S8_UINT,D3D11_BIND_DEPTH_STENCIL);
	result = Device->CreateTexture2D(&BufferDesc, nullptr,
		m_D3DTexture2D[EnumClassToInt(ERenderTextureType::DepthBuffer)].GetAddressOf());
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create DepthBufferTexture Failed."));
		return false;
	}

	// 깊이-스텐실 뷰를 생성. 
	WriteDepthStencilViewDesc(DepthViewDesc);
	result = Device->CreateDepthStencilView(m_D3DTexture2D[EnumClassToInt(ERenderTextureType::DepthBuffer)].Get(),
		&DepthViewDesc, m_DepthStencilView.GetAddressOf());
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create DepthStencilView Failed."));
		return false;
	}
	return true;
}
bool JGRenderTargetD::CreateRenderTarget(ID3D11Device* Device, const int Width, const int Height,
	const ERenderTargetType type, const DXGI_FORMAT& format, UINT BindFlags, D3D11_USAGE Usage)
{
	// 나중에 작성
	// 장치 초기화가 우선

	return true;
}


ID3D11RenderTargetView* JGRenderTargetD::Get()
{
	if (m_RenderTarget.Get())
	{
		return m_RenderTarget.Get();
	}
	JGLog::Write(ELogLevel::Warning, TT("RenderTarget is nullptr."));
	return nullptr;
}
ID3D11RenderTargetView ** JGRenderTargetD::GetAddress()
{
	return m_RenderTarget.GetAddressOf();
}
ID3D11ShaderResourceView* JGRenderTargetD::GetTexture()
{
	if (m_ShaderResource.Get())
	{
		return m_ShaderResource.Get();
	}
	JGLog::Write(ELogLevel::Warning, TT("ShaderResourceView of JGRenderTargetD is nullptr."));
	return nullptr;
}
ID3D11Texture2D* JGRenderTargetD::GetD3DTexture2D(const ERenderTextureType type)
{
	if (m_D3DTexture2D[static_cast<int>(type)].Get())
	{
		return m_D3DTexture2D[static_cast<int>(type)].Get();
	}
	JGLog::Write(ELogLevel::Warning, TT("D3DTexture2D of JGRenderTargetD is nullptr."));
	return nullptr;
}
ID3D11DepthStencilView* JGRenderTargetD::GetDepthStencilView()
{
	if (m_DepthStencilView.Get())
	{
		return m_DepthStencilView.Get();
	}
	JGLog::Write(ELogLevel::Warning, TT("DepthStencilView of JGRenderTargetD is nullptr."));
	return nullptr;
}
ID3D11DepthStencilView ** JGRenderTargetD::GetDepthStencilViewAddress()
{
	return m_DepthStencilView.GetAddressOf();
}
void JGRenderTargetD::WriteTextureDesc(D3D11_TEXTURE2D_DESC& Desc, const DXGI_FORMAT& format,
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

void JGRenderTargetD::WriteDepthStencilViewDesc(D3D11_DEPTH_STENCIL_VIEW_DESC& Desc)
{
	// 깊이-스텐실 뷰의 서술자 작성
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	Desc.Texture2D.MipSlice = 0;
}
