#pragma once
#include"JGRCObject.h"






namespace JGRC
{
	class CORE_EXPORT DxWindow : public JGRCObject
	{
		/* 윈도우 하나에 필요한 것들
		윈도우 핸들
		스왑 체인
		렌더 타겟
		렌더 상태
		뷰포트
		*/
		typedef std::function<void()> DrawEvent;
	private:
		class DirectX* m_Dx = nullptr;
	private:
		HWND m_hWnd;
		std::unique_ptr<class JGSwapChain>    m_SwapChain;
		std::unique_ptr<class JGRenderTarget> m_RenderTarget;
		std::unique_ptr<class JGViewport>     m_Viewport;
	private:
		std::vector<DrawEvent> m_SubEvent;
		std::vector<DrawEvent> m_MainEvent;
		jgVec4 m_BackColor;
	public:
		DxWindow();
		virtual ~DxWindow();
		bool Init(const JGInitConfig& config);
		void Draw();
	public:
		void ResigisterRenderTarget();
	public:
		void AddSubEvent(const DrawEvent& evt)  { m_SubEvent.push_back(evt); }
		void AddMainEvent(const DrawEvent& evt) { m_MainEvent.push_back(evt); }
	public:
		HWND GetHandle() { return m_hWnd; }
		jgMatrix4x4& GetProjectionMatrix();
		jgMatrix4x4& GetOrhthoMatrix();
		std::vector<DrawEvent>* GetSubEvent()  { return &m_SubEvent; }
		std::vector<DrawEvent>* GetMainEvent() { return &m_MainEvent; }
	};
}




