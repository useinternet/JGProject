#pragma once
#include"../EngineStatics/Engine.h"


// ID3D11DepthStencilState 2°³
// ID3D11BlendState 2°³
// ID3D11RasterizerState
// 	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState.Get(), 1);
// m_DeviceContext->RSSetState(m_RasterState.Get());
enum class ENGINE_EXPORT EDepthStateType
{
	ZBufferOn  = 0,
	ZBufferOff = 1
};
enum class ENGINE_EXPORT EBlendStateType
{
	BlendOn  = 0,
	BlendOff = 1
};
enum class ENGINE_EXPORT EFillModeType
{
	Solid     = 0,
	WireFrame = 1
};
class ENGINE_EXPORT JGRenderSystemStateD
{
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthState_Zon;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthState_Zoff;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendState_on;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendState_off;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterState_FS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterState_WF;
public:
	bool InitRenderSystemSetting(ID3D11Device* Device);
	ID3D11DepthStencilState* GetDepthState(const EDepthStateType type);
	ID3D11BlendState* GetBlendState(const EBlendStateType type);
	ID3D11RasterizerState* GetRasterizerState(const EFillModeType type);
private:
	void WriteDepthStateDesc(D3D11_DEPTH_STENCIL_DESC& Desc,const EDepthStateType type);
	void WriteBlendStateDesc(D3D11_BLEND_DESC& Desc,const EBlendStateType type);
	void WriteRasterStateDesc(D3D11_RASTERIZER_DESC& Desc,const EFillModeType type);
};

