#pragma once
#include"Engine.h"




class JMainTimer;
class JThreadManager;
class JGLog;
class JGInputEvent;
class RenderSystem;
class SoundSystem;
class InputSystem;
class JGSuperClass;
class JGConfigLoaderManager;
class JGFontLoader;
class JGConstructHelper;
class GameLoop;

class ENGINE_EXPORT JGEngineMain
{
private: // 초기화 우선순위 별 

	JMainTimer*     m_EngineTimer;
	JThreadManager* m_ThreadManager;
	std::unique_ptr<JGLog>        m_EngineLog;
	std::unique_ptr<JGInputEvent> m_InputEvent;
	std::unique_ptr<RenderSystem> m_RenderSystem;
	std::unique_ptr<SoundSystem>  m_SoundSystem;
	std::unique_ptr<InputSystem>  m_InputSystem;
	std::unique_ptr<JGSuperClass> m_SuperClass;
	std::unique_ptr<JGConfigLoaderManager> m_ConfigManager;
	std::unique_ptr<JGFontLoader> m_FontLoader;
	std::unique_ptr<JGConstructHelper> m_ConstructHelper;
	std::unique_ptr<GameLoop>     m_GameLoop;
	bool m_bPaused = false;
public:
	JGEngineMain();
	~JGEngineMain();
	bool Init(HINSTANCE Instance,HWND hWnd);
	void Run();
	void DoEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void Destroy();
};