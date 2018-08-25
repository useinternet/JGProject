#pragma once
#include"../EngineStatics/Engine.h"

// 추가할거
// 렌더타겟 뷰와 깊이-스텐실 버퍼를 각각 출력 파이프라인에 바인딩합니다.
//m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());



/* 
Enum Class : ERenderTextureTypew
@e RenderTarget : 렌더 타깃용 텍스쳐
@e DepthBuffer  : 깊이 버퍼용 텍스쳐
*/
enum class ENGINE_EXPORT ERenderTextureType
{
	RenderTarget = 0,
	DepthBuffer  = 1
};
/*
EnumClass : ERenderTargetType
@e NoneDepth : 깊이버퍼텍스쳐를 만들지 않는다.
@e Depth     : 깊이버퍼텍스쳐를 만든다.
*/
enum class ENGINE_EXPORT ERenderTargetType
{
	NoneDepth           = 0,
	Depth               = 1,
};
/*
Class : JGRenderTargetD
@m RenderTargetView   m_RenderTarget      : 렌더 타깃
@m ShaderResourceView m_ShaderResource    : 셰이더 리소스 뷰(최종 텍스쳐)
@m Texture2DArray     m_D3DTexture2D      : 텍스쳐 배열 (그릴 도화지들)
@m DepthSV            m_DepthStencilView  : 깊이 스텐실 뷰
@m ERenderTargetType  m_RenderTargetType  : 현재 클래스 렌더타겟 타입
@m int m_Width  : 그리는 텍스쳐 가로 길이
@m int m_Height : 그리는 텍스쳐 세로 길이
*/
class ENGINE_EXPORT JGRenderTargetD
{
	typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView>        RenderTargetView;
	typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>      ShaderResourceView;
	typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView>        DepthSV;
	typedef std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>>  Texture2DArray;

private:
	RenderTargetView   m_RenderTarget;
	ShaderResourceView m_ShaderResource;
	Texture2DArray     m_D3DTexture2D;
	DepthSV            m_DepthStencilView;
	ERenderTargetType  m_RenderTargetType;

	int m_Width = 0;
	int m_Height = 0;
public:
	JGRenderTargetD();
	~JGRenderTargetD();
	/*
	Exp : 렌더타겟을 생성한다. ( 메인 장치 용 )
	@param ID3D11Device*   Device     : Direct 디바이스
	@param IDXGISwapChain* SwapChain  : 스왑체인 
	@param const int       Width      : 그릴 가로 길이
	@param const int       Height     : 그릴 세로 길이
	*/
	bool CreateRenderTarget(ID3D11Device* Device,IDXGISwapChain* SwapChain,const int Width,const int Height);
	/*
	Exp : 렌더타겟을 생성한다. ( 메인 장치 용 )
	@param ID3D11Device*   Device     : Direct 디바이스
	@param const int       Width      : 그릴 가로 길이
	@param const int       Height     : 그릴 세로 길이
	@param const ERenderTargetType& type   : 렌더타겟 타입
	@param const DXGI_FORMAT&       format : 그릴 형식
	@param UINT        BindFlags : 바인딩할 플래그
	@param D3D11_USAGE Usage     : Usage
	*/
	bool CreateRenderTarget(ID3D11Device* Device, const int Width, const int Height, const ERenderTargetType type,
		const DXGI_FORMAT& format,UINT BindFlags, D3D11_USAGE Usage = D3D11_USAGE_DEFAULT);
	/*
	Exp : 렌더타겟 포인터를 취득한다. */
	ID3D11RenderTargetView*    Get();
	/*
	Exp : 렌더타겟 포인터 주소를 취득한다. */
	ID3D11RenderTargetView**   GetAddress();
	/*
	Exp :  자원리소스(최종 텍스쳐)를 얻는다.*/
	ID3D11ShaderResourceView*  GetTexture();
	/*
	Exp :  텍스쳐를 얻는다.
	@param : const ERenderTextureType& type : 텍스쳐 타입 */
	ID3D11Texture2D*        GetD3DTexture2D(const ERenderTextureType type);
	/*
	Exp :  깊이 스텐실 뷰 포인터를 취득한다. */
	ID3D11DepthStencilView*  GetDepthStencilView();
	/*
	Exp :  깊이 스텐실 뷰 포인터 주소를 취득한다. */
	ID3D11DepthStencilView** GetDepthStencilViewAddress();

private:
	void WriteTextureDesc(D3D11_TEXTURE2D_DESC& Desc, const DXGI_FORMAT& format,
		UINT BindFlags, D3D11_USAGE Usage = D3D11_USAGE_DEFAULT);
	void WriteDepthStencilViewDesc(D3D11_DEPTH_STENCIL_VIEW_DESC& Desc);
};
