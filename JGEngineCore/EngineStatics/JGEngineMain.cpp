#include"JGEngineMain.h"
#include"JTimerManager.h"
#include"JThreadManager.h"
#include"JGLog.h"
#include"JGInputEvent.h"
#include"../RenderSystem/RenderSystem.h"
#include"JGConstructHelper.h"
#include"../EngineFrameWork/GameLoop.h"
using namespace std;

JGEngineMain::JGEngineMain()
{
	m_EngineTimer   = JMainTimer::GetInstance();
	m_ThreadManager = JThreadManager::GetInstance();


	m_EngineLog    = make_unique<JGLog>();
	m_InputEvent   = make_unique<JGInputEvent>();
	m_RenderSystem = make_unique<RenderSystem>();
	m_ConstructHelper = make_unique<JGConstructHelper>();
	m_GameLoop = make_unique<GameLoop>();
}
JGEngineMain::~JGEngineMain() {}
bool JGEngineMain::Init(HWND hWnd)
{
	bool result = true;
	m_EngineTimer->Start();
	// 로그 초기화
	m_EngineLog->InitLog();

	// 렌더링 나머지 여부는 나중에 JSon파일로 받아와서 설정한다.
	result = m_RenderSystem->InitRenderSystem(hWnd, false, 1920, 1080, 45.0f, 1000.0f, 0.1f);
	if (!result)
	{
		//예외처리
		return false;
	}

	// 게임 루프 초기화
	m_GameLoop->InitGameLoop(m_RenderSystem.get());
	return true;
}
void JGEngineMain::Run()
{
	MSG msg;

	while (!m_bPaused)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			m_bPaused = true;
		}
		m_EngineTimer->Tick();
		m_GameLoop->Tick(m_EngineTimer->GetDeltaTime());
		m_RenderSystem->Render();
	}
}
void JGEngineMain::DoEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_InputEvent->DoEvent(message, wParam, lParam);
}
void JGEngineMain::Destroy()
{
	m_RenderSystem.reset();
	m_InputEvent.reset();
	m_EngineLog->Destory();
	m_ThreadManager->Destroy();
	m_EngineTimer->Destroy();
}