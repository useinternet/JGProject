#include"JGRenderState.h"
using namespace JGRC;

bool JGRenderState::RenderStateInit(ID3D11Device* Device)
{
	HRESULT result = S_OK;
	D3D11_DEPTH_STENCIL_DESC DepthStateDesc;
	D3D11_BLEND_DESC         BlendStateDesc;
	D3D11_RASTERIZER_DESC    RasterStateDesc;


	// ZBuffer On 모드 깊이 상태 생성
	WriteDepthStateDesc(DepthStateDesc, EDepthStateType::ZBufferOn);
	result = Device->CreateDepthStencilState(&DepthStateDesc, m_DepthState_Zon.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}
	// ZBuffer Off 모드 깊이 상태 생성
	WriteDepthStateDesc(DepthStateDesc, EDepthStateType::ZBufferOff);
	result = Device->CreateDepthStencilState(&DepthStateDesc, m_DepthState_Zoff.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	// Blend On 모드 블렌드 상태 생성
	WriteBlendStateDesc(BlendStateDesc, EBlendStateType::BlendOn);
	result = Device->CreateBlendState(&BlendStateDesc, m_BlendState_on.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}
	// Blend Off 모드 블렌드 상태 생성
	WriteBlendStateDesc(BlendStateDesc, EBlendStateType::BlendOff);
	result = Device->CreateBlendState(&BlendStateDesc, m_BlendState_off.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}


	// Solid Raster 모드 생성
	WriteRasterStateDesc(RasterStateDesc, EFillModeType::Solid);
	result = Device->CreateRasterizerState(&RasterStateDesc, m_RasterState_FS.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}



	// WireFrame Raster 모드 생성
	WriteRasterStateDesc(RasterStateDesc, EFillModeType::WireFrame);
	result = Device->CreateRasterizerState(&RasterStateDesc, m_RasterState_WF.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;
}
ID3D11DepthStencilState* JGRenderState::GetDepthState(const EDepthStateType type)
{
	switch (type)
	{
	case EDepthStateType::ZBufferOn:
		if (m_DepthState_Zon.Get())
		{
			return m_DepthState_Zon.Get();
		}
	case EDepthStateType::ZBufferOff:
		if (m_DepthState_Zoff.Get())
		{
			return m_DepthState_Zoff.Get();
		}
	default:
		return nullptr;
	}
}
ID3D11BlendState* JGRenderState::GetBlendState(const EBlendStateType type)
{
	switch (type)
	{
	case EBlendStateType::BlendOn:
		if (m_BlendState_on.Get())
		{
			return m_BlendState_on.Get();
		}
	case EBlendStateType::BlendOff:
		if (m_BlendState_off.Get())
		{
			return m_BlendState_off.Get();
		}
	default:
		return nullptr;
	}
}
ID3D11RasterizerState* JGRenderState::GetRasterizerState(const EFillModeType type)
{
	switch (type)
	{
	case EFillModeType::Solid:
		if (m_RasterState_FS.Get())
		{
			return m_RasterState_FS.Get();
		}
	case EFillModeType::WireFrame:
		if (m_RasterState_WF.Get())
		{
			return m_RasterState_WF.Get();
		}
	default:
		return nullptr;
	}
}
void JGRenderState::WriteDepthStateDesc(D3D11_DEPTH_STENCIL_DESC& Desc,const EDepthStateType type)
{
	// 스텐실 상태의 서술자 작성
	ZeroMemory(&Desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	if (type == EDepthStateType::ZBufferOn)   Desc.DepthEnable = true;
	if (type == EDepthStateType::ZBufferOff)  Desc.DepthEnable = false;
	Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	Desc.DepthFunc      = D3D11_COMPARISON_LESS;

	Desc.StencilEnable    = true;
	Desc.StencilReadMask  = 0xFF;
	Desc.StencilWriteMask = 0xFF;

	Desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
	Desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	Desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
	Desc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
	Desc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
	Desc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR;
	Desc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
	Desc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;
}

void JGRenderState::WriteBlendStateDesc(D3D11_BLEND_DESC& Desc, const EBlendStateType type)
{
	ZeroMemory(&Desc, sizeof(D3D11_BLEND_DESC));

	if (type == EBlendStateType::BlendOn) Desc.RenderTarget[0].BlendEnable = true;
	if (type == EBlendStateType::BlendOff) Desc.RenderTarget[0].BlendEnable = false;
	Desc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	Desc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	Desc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ZERO;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL & ~D3D11_COLOR_WRITE_ENABLE_ALPHA;//0x0f;
}

void JGRenderState::WriteRasterStateDesc(D3D11_RASTERIZER_DESC& Desc, const EFillModeType type)
{
	if( type == EFillModeType::Solid)     Desc.FillMode = D3D11_FILL_SOLID;
	if( type == EFillModeType::WireFrame) Desc.FillMode = D3D11_FILL_WIREFRAME;
	Desc.CullMode              = D3D11_CULL_BACK;
	Desc.AntialiasedLineEnable = false;
	Desc.DepthBias             = 0;
	Desc.DepthBiasClamp        = 0.0f;
	Desc.DepthClipEnable       = true;
	Desc.FrontCounterClockwise = false;
	Desc.MultisampleEnable     = false;
	Desc.ScissorEnable         = false;
	Desc.SlopeScaledDepthBias  = 0.0f;
}
