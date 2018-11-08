#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"

/*
// 임시 적용
m_Device->GetContext()->OMSetRenderTargets(1, m_RenderTarget->GetAddress(),
m_RenderTarget->GetDepthStencilView());
m_Device->GetContext()->OMSetDepthStencilState(m_RenderState->GetDepthState(EDepthStateType::ZBufferOn), 1);
m_Device->GetContext()->RSSetState(m_RenderState->GetRasterizerState(EFillModeType::Solid));
m_Device->GetContext()->RSSetViewports(1, m_Viewport->Get());
*/

namespace JGRC
{
	class JGRenderTarget;
	class JGRenderState;
	class JGViewport;
	class CORE_EXPORT JGDevice
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11Device>        m_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
	public:
		JGDevice();
		~JGDevice();
		/*
		Exp : 장치를 생성한다.*/
		bool CreateDevice();
		/*
		Exp : 렌더 타겟 뷰를 장치에 바인딩 합니다*/
		void BindRederTarget(JGRenderTarget* renderTarget, const UINT numViews = 1);
		/*
		Exp : 깊이 스텐실 상태를 바인딩 한다. */
		void BindDepthStencilState(JGRenderState* state, const EDepthStateType type, const UINT numDepths = 1);
		/*
		Exp : 블렌딩 상태를 바인딩 한다. */
		void BindBlendState(JGRenderState* state, const EBlendStateType type, const float blendFactor[4], const UINT SampleMask = 0xffffffff);
		/*
		Exp : 레스터 상태를 바인딩 한다. */
		void BindRasterizerState(JGRenderState* state,const EFillModeType type);
		/*
		Exp : 뷰포트를 바인딩한다. */
		void BindViewport(JGViewport* viewport, const UINT numVps = 1);

		/*
		Exp : ID3D11Device 포인터를 취득한다 */
		ID3D11Device* GetDevice();
		/*
		Exp : ID3D11DeviceContext 포인터를 취득한다 */
		ID3D11DeviceContext* GetContext();

	
	};


}






