#include "pch.h"
#include "JGUIScreen.h"
#include "Class/JWindow.h"
#include "JGUIObject/JGUIWindow.h"
#include "Object/DxObject/DxScreen.h"
#include "Object/Shader/ShaderModule.h"
#include "Object/DxObject/RenderDevice.h"
#include "Object/ReObject/ReCamera.h"
#include "RenderEngine.h"

using namespace std;
using namespace RE;
JGUIScreen::JGUIScreen(const std::string& name) : m_Name(name)
{
	m_DxScreen = nullptr;
	m_Window   = nullptr;
}
JGUIScreen::~JGUIScreen()
{
	RE::RenderEngine::DestroyDxScreen(m_DxScreen);
	JWindowManager::Destroy(m_Window);
}
bool JGUIScreen::Init(uint32_t width, uint32_t height, uint32_t startX, uint32_t startY)
{
	if (width == 0)
		width = 800;
	if (height == 0)
		height = 600;

	JWindowDesc win_desc = {};
	win_desc.width  = width;
	win_desc.height = height;
	win_desc.name = m_Name;
	win_desc.parent_hWnd = 0;
	if (JGUI::GetMainWindow())
	{
		win_desc.parent_hWnd = JGUI::GetMainWindow()->GetRootWindowHandle();
	}
	


	m_Window = JWindowManager::Create(win_desc, startX, startY);
	if (m_Window == nullptr)
		return false;

	m_DxScreen = RE::RenderEngine::CreateDxScreen(m_Window->GetHandle(), width, height);
	if (m_DxScreen == nullptr)
		return false;
	return true;
}

void JGUIScreen::Resize(uint32_t width, uint32_t height)
{
	m_DxScreen->ReSize(width, height);
	m_Window->SetClientSize(width, height);
}

HWND JGUIScreen::GetHandle() const
{
	return m_Window->GetHandle();
}

void JGUIScreen::BindGUIModuleClone(RE::FixedGShaderModuleClone* clone)
{
	if (m_DxScreen)
	{
		m_DxScreen->SetFinalTexture(clone->GetRTTexture(0));
	}
}
