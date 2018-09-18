#pragma once
#include"../EngineStatics/Engine.h"



class JGSuperClass;
/*
Class Name : RenderSystem
@m unique_ptr<class JGDeviceD>            m_Device              : Direct 장치
@m unique_ptr<class JGSwapChainD>         m_SwapChain           : 스왑체인
@m unique_ptr<class JGRenderTargetD>      m_RenderTarget        : 메인 렌더타겟
@m unique_ptr<class JGRenderSystemStateD> m_RenderState         : 렌더링시스템 상태
@m unique_ptr<class JGViewportD>          m_Viewport            : 뷰포트
@m unique_ptr<class JGBufferManager>      m_JGBufferManager     : 버퍼 매니저( 상수버퍼, 정점 버퍼, 인덱스 버퍼)
@m unique_ptr<class JBHLSLShaderDevice>   m_ShaderDevice        : hlsl 장치( 셰이더 관리 장치)
@m unique_ptr<class JGShaderConstructor>  m_ShaderConstructor   : 셰이더 생성 보조 클래스
@m unique_ptr<class JGSuperClass>   m_SuperClass          : 렌더링에 필요한 장치들의 주소값을 담은 클래스(전달용)
@m unique_ptr<class Object>               m_ObjectConstructInit : 초기화용 오브젝트
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
	std::unique_ptr<class JGBufferManager>      m_JGBufferManager;
	std::unique_ptr<class JGHLSLShaderDevice>   m_ShaderDevice;
	std::unique_ptr<class JGShaderConstructor>  m_ShaderConstructor;
	std::unique_ptr<class Object>               m_ObjectConstructInit;
	std::list<std::shared_ptr<Object>>* m_RenderingObjectArray;
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

	/*
	Exp : 그리기  */
	void Render();

	void InitObjectProtoType(JGSuperClass* SuperClass);
	void ReceiveObjectArray(std::list<std::shared_ptr<Object>>* Array);

	JGDeviceD*          GetDevice();
	JGViewportD*        GetViewPort();
	JGHLSLShaderDevice* GetShaderDevice();
	JGBufferManager*    GetBufferManager();


private:
	//임시
	void ApplicationInDeviceContext();
};