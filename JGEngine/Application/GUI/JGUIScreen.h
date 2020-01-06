#pragma once

#include "JGUI.h"


namespace RE {
	class DxScreen;
	class GUIModule;
	class ReCamera;
	class FixedGShaderModuleClone;
	class Texture;
}
class JWindow;

class JGUIScreen
{
public:
	JGUIScreen(const std::string& name);
	~JGUIScreen();
	bool Init(uint32_t width, uint32_t height, uint32_t startX, uint32_t startY);
	void Resize(uint32_t width, uint32_t height);
	HWND GetHandle() const;
	void BindGUIModuleClone(RE::FixedGShaderModuleClone* clone);

	JWindow* GetJWin() const {
		return m_Window;
	}
private:
	RE::DxScreen* m_DxScreen = nullptr;
	JWindow*      m_Window   = nullptr;
	std::string   m_Name;
};