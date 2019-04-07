#pragma once
#include<imgui/imgui.h>
#include<string>

namespace JgGui
{
	class GuiComponent
	{
		
	protected:
		std::string m_GuiName;
		float m_X = -1.0f, m_Y = -1.0f;
		float m_sizeX = -1.0f, m_sizeY = -1.0f;
		ImGuiCond_ m_ImGuiSetPosFlag = ImGuiCond_Once;
		ImGuiCond_ m_ImGuiSetSizeFlag = ImGuiSetCond_Once;
		bool  m_IsVisible = true;
	public:
		GuiComponent(const std::string Name = "GuiComponent") : m_GuiName(Name) {}
		virtual ~GuiComponent() {}
	public:
		virtual void NewFrame() {
			if (m_X != -1 && m_Y != -1)
			{
				ImGui::SetWindowPos({ m_X,m_Y }, m_ImGuiSetPosFlag);

			}
			if (m_sizeX != -1 && m_sizeY != -1)
			{
				ImGui::SetWindowSize({ m_sizeX, m_sizeY }, m_ImGuiSetSizeFlag);
			}
		}
	public:
		const std::string& GetName() const {
			return m_GuiName;
		}
		void SetSize(float x, float y) {
			m_sizeX = x;
			m_sizeY = y;
		}
		void SetPos(float x, float y) {
			m_X = x;
			m_Y = y;
		}
		void SetName(const std::string& name) {
			m_GuiName = name;
		}
		void Hide() {
			m_IsVisible = false;
		}
		void Visible() {
			m_IsVisible = true;
		}
		void Toggle() {
			(m_IsVisible) ?
				m_IsVisible = false :
				m_IsVisible = true;
		}
		bool IsVisible() {
			return m_IsVisible;
		}
		float GetPosX() const {
			return m_X;
		}
		float GetPosY() const {
			return m_Y;
		}
		float GetSizeX() const {
			return m_sizeX;
		}
		float GetSizeY() const {
			return m_sizeY;
		}
	};
}





