#include "pch.h"
#include "Application.h"
#include "Class/Log.h"
#include "Event/Event.h"
// engine
#include "Game.h"
#include "RenderEngine.h"
#include "InputEngine.h"
#include "PhysicsEngine.h"
#include "SoundEngine.h"
#include "GUI/JGUI.h"
#include "GUI/JGUIObject/JGUIWindow.h"
using namespace std;
using namespace concurrency;

static GlobalLinkStream g_link_stream;
const GlobalLinkStream& Application::GetLinkStream()
{
	return g_link_stream;
}

Application::Application(const std::wstring& name, EApplicationMode mode) :
	m_AppMode(mode), m_AppName(name), m_IsInit(false){  }
Application::~Application()
{
	m_Graphics->Destroy();
	delete m_Graphics;
	m_Graphics = nullptr;
	m_Plugin.~Plugin();
}

bool Application::Init()
{
	Log::Init("JGEngine", "enginelog.txt");
	JWindowManager::Init();
	m_EventManager = make_shared<EventManager>();
	m_EngineTimer  = make_shared<EngineTimer>();
	m_EngineConfig = make_shared<EngineConfig>();
	m_Performance = make_shared<EnginePerformance>();
	m_EngineTimer->Start();
	m_EngineConfig->LoadConfig();

	// Create Global Link Stream 
	{
		auto call_back_func = std::bind(&Application::OnEvent, this, std::placeholders::_1);
		g_link_stream.LogFileName = "enginelog.txt";
		g_link_stream.Logger = Log::GetLogger();
		g_link_stream.OnEvent = call_back_func;
		g_link_stream.EngineEventManager = m_EventManager;
		g_link_stream._EngineTimer = m_EngineTimer;
		g_link_stream._EngineConfig = m_EngineConfig;
		g_link_stream._EnginePerformance = m_Performance;
		GlobalLinkData::Init(g_link_stream, true);
	}

	m_InputEngine = make_shared<IE::InputEngine>(g_link_stream);
	m_PhysicsEngine = make_shared<PE::PhysicsEngine>(g_link_stream);
	m_SoundEngine = make_shared<SE::SoundEngine>(g_link_stream);
	m_RenderEngine = make_shared<RE::RenderEngine>(g_link_stream);
	m_Game = make_shared<GFW::Game>(g_link_stream);

	m_RenderEngine->Init();

	m_IsInit = true;


	// Test
	AppDesc desc;
	m_Plugin.Load(desc.GraphicsEngineDllPath);
	m_UnitTestPlugin.Load(desc.GraphicsEngineDllPath);






	return true;
}
void Application::Load()
{
	m_InputEngine->Load();
	m_PhysicsEngine->Load();
	m_SoundEngine->Load();
	m_RenderEngine->Load();
	//
	m_Game->Load();
	m_GUI = make_shared<JGUI>(m_InputEngine.get());






	//JWindowDesc windesc;
	//windesc.width = 1100;
	//windesc.height = 618;
	//windesc.name = "UnitTest";
	//auto win = JWindowManager::Create(windesc, 0, 0);
	//((TestFunc)m_UnitTestPlugin.GetProcAddress("BindUnitTest"))(win->GetHandle(), 1920, 1080);
	//m_Graphics = ((CreateEngineCoreFunc)m_Plugin.GetProcAddress("CreateGraphicsEngine"))(g_link_stream);
	//m_Graphics->Init();
	//m_Graphics->Load();
}
typedef	JGUIObject*(*TestFunc)(const std::string);
void Application::Run()
{
	MSG msg = { 0 };
	Load();
	while (msg.message != WM_QUIT)
	{
		JGUI::ClearExpectedDestroyScreen();
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
			auto game_task = make_task([&] {
				m_Game->Update();
			});
			auto gui_task = make_task([&] {
				m_GUI->Update();
			});
			auto render_task = make_task([&] {
				m_RenderEngine->Update();
				//m_Graphics->Update();
			});
			////엔진 업데이트

			structured_task_group _tasks;
			structured_task_group render_tasks;
			_tasks.run_and_wait(input_task);
			_tasks.run(physics_task);
			_tasks.run(sound_task);
			_tasks.run(game_task);
			_tasks.run(gui_task);
			_tasks.wait();

			_tasks.run_and_wait(render_task);
			m_EngineTimer->Tick();
			if (m_InputEngine->GetKeyUp(KeyCode::Esc))
			{
				auto win = JGUI::FindRootJGUIWindow(GetFocus());
				JGUI::DestroyObject(win);
			}

		}
	}
}
void Application::OnEvent(Event& e)
{

	if (!m_IsInit)
		return;
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
