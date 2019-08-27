#pragma once



#include "JE.h"
#include "GUI.h"
#include "EditorGUI/IGWindow.h"

namespace JE
{
	class IGManager;
	class EditorGUI : public RE::GUI
	{
	public:
		EditorGUI();
		virtual ~EditorGUI() = default;
	public:
		virtual void Init(RE::RenderDevice* renderDevice) override;
		void Load();
		virtual void Destroy() override;
		virtual void Render(ID3D12GraphicsCommandList* cmdList) override;
		virtual void OnEvent(Event& e) override;
	public:
		void Update();
	private:
		void ApplyDockSpace();
	private:
		HWND m_hWnd;

		RE::RenderDevice* m_pRenderDevice;
		std::shared_ptr<IGManager> m_IGManager;
		std::vector<std::string> m_InitWindowArray;
	};
}