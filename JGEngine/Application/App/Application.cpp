#include "pch.h"
#include "Application.h"
#include "Window.h"
#include "Class/Log.h"
#include "Event/Event.h"

// engine
#include "Game.h"
#include "RenderEngine.h"
#include "InputEngine.h"
#include "PhysicsEngine.h"
#include "SoundEngine.h"

// TEST
#include "GUI/EditorGUI.h"
using namespace std;
using namespace concurrency;
Application::Application(const std::wstring& name, EApplicationMode mode) :
	m_AppMode(mode), m_AppName(name), m_IsInit(false){  }
Application::~Application()
{
}
bool Application::Init()
{
	Log::Init("JGEngine", "enginelog.txt");
	m_EventManager = make_shared<EventManager>();
	m_EngineTimer  = make_shared<EngineTimer>();
	m_EngineConfig = make_shared<EngineConfig>();
	m_EngineTimer->Start();
	m_EngineConfig->LoadConfig();

	GlobalLinkStream stream;
	// Create Global Link Stream 
	{
		auto call_back_func = std::bind(&Application::OnEvent, this, std::placeholders::_1);
		stream.LogFileName = "enginelog.txt";
		stream.Logger = Log::GetLogger();
		stream.OnEvent = call_back_func;
		stream.EngineEventManager = m_EventManager;
		stream._EngineTimer = m_EngineTimer;
		stream._EngineConfig = m_EngineConfig;
		GlobalLinkData::Init(stream, true);
	}


	m_Window = make_shared<JE::Window>();
	m_InputEngine = make_shared<IE::InputEngine>(stream);
	m_PhysicsEngine = make_shared<PE::PhysicsEngine>(stream);
	m_SoundEngine = make_shared<SE::SoundEngine>(stream);
	m_RenderEngine = make_shared<RE::RenderEngine>(stream);
	m_Game = make_shared<GFW::Game>(stream);
	m_EditorGUI = make_shared<JE::EditorGUI>();


	////// 초기화
	if (!m_Window->Init(GetModuleHandle(NULL), m_AppName.c_str()))
	{
		return false;
	}

	auto windata = m_Window->GetData();
	RECT rect;
	GetClientRect(windata.hWnd, &rect);
	m_RenderEngine->Init(
		windata.hWnd, rect.right, rect.bottom, m_EditorGUI);
	m_InputEngine->Init(m_Window->GetData().hWnd);

	m_IsInit = true;
	return true;
}
void Application::Load()
{
	m_InputEngine->Load();
	m_PhysicsEngine->Load();
	m_SoundEngine->Load();
	m_RenderEngine->Load();
	m_Game->Load();
	m_EditorGUI->Load();
}
void Application::Run()
{
	MSG msg = { 0 };
	Load();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

			auto input_task = make_task([&] {
				m_InputEngine->Update();
			});
			auto physics_task = make_task([&] {
				m_PhysicsEngine->Update();
			});
			auto sound_task = make_task([&] {
				m_SoundEngine->Update();
			});
			auto gui_task = make_task([&] {
				m_EditorGUI->Update();
			});
			auto game_task = make_task([&] {
				m_Game->Update();
			});
			auto render_task = make_task([&] {
				m_RenderEngine->Update();
			});
			////엔진 업데이트

			structured_task_group update_tasks;
			structured_task_group render_tasks;
			update_tasks.run(input_task);
			update_tasks.run(physics_task);
			update_tasks.run(sound_task);
			update_tasks.run(game_task);
			update_tasks.run(gui_task);
			update_tasks.wait();

			render_tasks.run(render_task);
			render_tasks.wait();
			m_EngineTimer->Tick();
		}
	}
}
void Application::OnEvent(Event& e)
{

	if (!m_IsInit)
		return;
//	ENGINE_LOG_TRACE(e.ToString());
	if (e.IsInCategory(EventCategory_Application))
	{
		m_EditorGUI->OnEvent(e);
	}
	if (e.IsInCategory(EventCategory_RenderEngine))
	{
		m_RenderEngine->OnEvent(e);
	}
	if (e.IsInCategory(EventCategory_Game))
	{
		m_Game->OnEvent(e);
	}
	if (e.IsInCategory(EventCategory_InputEngine))
	{
		m_InputEngine->OnEvent(e);
	}
	if (e.IsInCategory(EventCategory_SoundEngine))
	{
		m_SoundEngine->OnEvent(e);
	}
	if (e.IsInCategory(EventCategory_PhysicsEngine))
	{
		m_PhysicsEngine->OnEvent(e);
	}
}
