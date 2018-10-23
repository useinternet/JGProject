#pragma once
#include"Common/JGRCCommon.h"

namespace JGRenderCore
{
	/*
	EnumClass : EDepthStateType
	@e ZBufferOn  : Z축을 On한다.
	@e ZBufferOff : Z축을 Off한다.*/
	enum class CORE_EXPORT EDepthStateType
	{
		ZBufferOn = 0,
		ZBufferOff = 1
	};
	/*
	EnumClass : EBlendStateType
	@e BlendOn  : 알파 값을 적용한다.
	@e BlendOff : 알파 값을 적용하지 않는다.*/
	enum class CORE_EXPORT EBlendStateType
	{
		BlendOn = 0,
		BlendOff = 1
	};
	/*
	EnumClass : EFillModeType
	@e Solid     : 정점 사이에 색을 모두 채운다.
	@e WireFrame : 정점 사이에 색을 모두 채우지 않는다.*/
	enum class CORE_EXPORT EFillModeType
	{
		Solid = 0,
		WireFrame = 1
	};
	/*
	Class : JGRenderSystemStateD
	@m ComPtr<ID3D11DepthStencilState> m_DepthState_Zon  : 깊이 상태( Zon 모드)
	@m ComPtr<ID3D11DepthStencilState> m_DepthState_Zoff : 깊이 상태( Zoff 모드 )
	@m ComPtr<ID3D11BlendState>        m_BlendState_on   : 블렌딩 상태 ( On 모드 )
	@m ComPtr<ID3D11BlendState>        m_BlendState_off  : 블렌딩 상태( Off 모드 )
	@m ComPtr<ID3D11RasterizerState>   m_RasterState_FS  : 레이스터 상태 ( Solid 모드 )
	@m ComPtr<ID3D11RasterizerState>   m_RasterState_WF  : 레이스터 상태 ( WireFrame 모드 )
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
		Exp : 렌더시스템 세팅을 초기화한다.
		@param ID3D11Device* Device : DirectX 장치(JGDevice->GetDevice) */
		bool RenderStateInit(ID3D11Device* Device);
		/*
		Exp : 깊이 상태를 가져온다.
		@param const EDepthStateType : 깊이 상태 타입 */
		ID3D11DepthStencilState* GetDepthState(const EDepthStateType type);
		/*
		Exp : 블렌딩 상태를 가져온다.
		@param const EBlendStateType : 블렌딩 상태 타입 */
		ID3D11BlendState* GetBlendState(const EBlendStateType type);
		/*
		Exp : 레이스터 상태를 가져온다.
		@param const EFillModeType : 레이스터 상태 타입 */
		ID3D11RasterizerState* GetRasterizerState(const EFillModeType type);
	private:
		void WriteDepthStateDesc(D3D11_DEPTH_STENCIL_DESC& Desc, const EDepthStateType type);
		void WriteBlendStateDesc(D3D11_BLEND_DESC& Desc, const EBlendStateType type);
		void WriteRasterStateDesc(D3D11_RASTERIZER_DESC& Desc, const EFillModeType type);
	};

}

