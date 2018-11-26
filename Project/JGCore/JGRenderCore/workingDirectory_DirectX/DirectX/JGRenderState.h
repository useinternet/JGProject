#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
namespace JGRC
{
	/*
	Class : JGRenderSystemStateD
	@m ComPtr<ID3D11DepthStencilState> m_DepthState_Zon  : ���� ����( Zon ���)
	@m ComPtr<ID3D11DepthStencilState> m_DepthState_Zoff : ���� ����( Zoff ��� )
	@m ComPtr<ID3D11BlendState>        m_BlendState_on   : ������ ���� ( On ��� )
	@m ComPtr<ID3D11BlendState>        m_BlendState_off  : ������ ����( Off ��� )
	@m ComPtr<ID3D11RasterizerState>   m_RasterState_FS  : ���̽��� ���� ( Solid ��� )
	@m ComPtr<ID3D11RasterizerState>   m_RasterState_WF  : ���̽��� ���� ( WireFrame ��� )
	@m*/
	class CORE_EXPORT JGRenderState
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthState_Zon;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthState_Zoff;
		Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendState_on;
		Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendState_off;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterState_FS;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterState_WF;
	public:
		/*
		Exp : �����ý��� ������ �ʱ�ȭ�Ѵ�.
		@param ID3D11Device* Device : DirectX ��ġ(JGDevice->GetDevice) */
		bool RenderStateInit(ID3D11Device* Device);
		/*
		Exp : ���� ���¸� �����´�.
		@param const EDepthStateType : ���� ���� Ÿ�� */
		ID3D11DepthStencilState* GetDepthState(const EDepthStateType type);
		/*
		Exp : ������ ���¸� �����´�.
		@param const EBlendStateType : ������ ���� Ÿ�� */
		ID3D11BlendState* GetBlendState(const EBlendStateType type);
		/*
		Exp : ���̽��� ���¸� �����´�.
		@param const EFillModeType : ���̽��� ���� Ÿ�� */
		ID3D11RasterizerState* GetRasterizerState(const EFillModeType type);
	private:
		void WriteDepthStateDesc(D3D11_DEPTH_STENCIL_DESC& Desc, const EDepthStateType type);
		void WriteBlendStateDesc(D3D11_BLEND_DESC& Desc, const EBlendStateType type);
		void WriteRasterStateDesc(D3D11_RASTERIZER_DESC& Desc, const EFillModeType type);
	};

}
