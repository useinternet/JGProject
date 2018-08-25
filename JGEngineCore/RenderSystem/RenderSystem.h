#pragma once
#include"../EngineStatics/Engine.h"



/*
Class Name : RenderSystem
@m std::unique_ptr<class JGDeviceD>            m_Device       : Direct 장치
@m std::unique_ptr<class JGSwapChainD>         m_SwapChain    : 스왑체인
@m std::unique_ptr<class JGRenderTargetD>      m_RenderTarget : 메인 렌더타겟
@m std::unique_ptr<class JGRenderSystemStateD> m_RenderState  : 렌더링시스템 상태
@m std::unique_ptr<class JGViewportD>          m_Viewport     : 뷰포트
@m HWND m_hWnd           : 윈도우 핸들
@m int  m_ScreenWidth    : 스크린 가로 길이
@m int  m_ScreenHeight   : 스크린 세로 길이
@m bool m_bFullScreen    : 전체 화면 여부
*/
class ENGINE_EXPORT RenderSystem
{
private:
	std::unique_ptr<class JGDeviceD>            m_Device;
	std::unique_ptr<class JGSwapChainD>         m_SwapChain;
	std::unique_ptr<class JGRenderTargetD>      m_RenderTarget;
	std::unique_ptr<class JGRenderSystemStateD> m_RenderState;
	std::unique_ptr<class JGViewportD>          m_Viewport;


	HWND m_hWnd;
	int  m_ScreenWidth;
	int  m_ScreenHeight;
	bool m_bFullScreen;
public:
	RenderSystem();
	~RenderSystem();

	/*
	Exp : 렌더링시스템을 초기화 한다. 
	@param HWND hWnd               : 윈도우 핸들
	@param const bool bFullScreen  : 전체화면 여부
	@param const int ScreenWidth   : 스크린 가로 길이
	@param const int ScreenHeight  : 스크린 세로 길이
	@param const float FOV         : 화면상 보여지는 각도(Field Of View)
	@param const float FarZ        : 화면상 그려지는 Z축 최대 길이
	@param const float NearZ       : 화면상 그려지는 Z축 최소 길이 */ 
	bool InitRenderSystem(HWND hWnd, const bool bFullScreen, const int ScreenWidth, const int ScreenHeight,
		const float FOV, const float FarZ, const float NearZ);
	/*
	Exp : 그리기 시작 */
	void BeginRendering();
	/*
	Exp : 그리기 종료 */
	void EndRendering();


private:
	void ApplicationInDeviceContext();
};