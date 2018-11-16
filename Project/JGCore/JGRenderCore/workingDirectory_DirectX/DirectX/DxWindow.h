#pragma once
#include"JGRCObject.h"
namespace JGRC
{
	class CORE_EXPORT DxWindow : public JGRCObject
	{
		typedef std::function<void()> DrawEvent;
	private:
		class DirectX* m_Dx = nullptr;
	private:
		HWND m_hWnd;
		std::unique_ptr<class JGSwapChain>    m_SwapChain;
		std::unique_ptr<class JGRenderTarget> m_RenderTarget;
		std::unique_ptr<class JGViewport>     m_Viewport;
		std::unique_ptr<class SRSLightPass>   m_LightPass;
		std::unique_ptr<class SRSRenderTarget>      m_srsRenderTarget;
	private:
		std::vector<DrawEvent> m_RenderPassEvent;
		std::vector<DrawEvent> m_LightPassEvent;
		jgVec4 m_BackColor;
	public:
		DxWindow();
		virtual ~DxWindow();
		bool Init(const DxWinConfig& config);
		void Draw();
	public:
		void AddRenderPassEvent(const DrawEvent& evt)  { m_RenderPassEvent.push_back(evt); }
		void AddLightPassEvent(const DrawEvent& evt) { m_LightPassEvent.push_back(evt); }
	public:
		HWND GetHandle() { return m_hWnd; }
		jgMatrix4x4& GetProjectionMatrix();
		jgMatrix4x4& GetOrhthoMatrix();
		std::vector<DrawEvent>* GetSubEvent()  { return &m_RenderPassEvent; }
		std::vector<DrawEvent>* GetMainEvent() { return &m_LightPassEvent; }
	};
}




