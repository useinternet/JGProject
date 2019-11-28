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

using namespace std;
using namespace concurrency;
Application::Application(const std::wstring& name, EApplicationMode mode) :
	m_AppMode(mode), m_AppName(name), m_IsInit(false){  }
Application::~Application()
{
	//GlobalLinkData::Destory();
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
		stream._EnginePerformance = m_Performance;
		GlobalLinkData::Init(stream, true);
	}

	m_InputEngine = make_shared<IE::InputEngine>(stream);
	m_PhysicsEngine = make_shared<PE::PhysicsEngine>(stream);
	m_SoundEngine = make_shared<SE::SoundEngine>(stream);
	m_RenderEngine = make_shared<RE::RenderEngine>(stream);
	m_Game = make_shared<GFW::Game>(stream);


	////// 초기화
	JWindowDesc desc;
	desc.name = "JWindow";
	desc.width = 800;
	desc.height = 600;
	auto window = JWindowManager::Create(desc, 100, 50);
	JWindowManager::SetMainWindow(window);


	m_RenderEngine->Init(
		window->GetHandle(), desc.width, desc.height);
	m_InputEngine->Init(window->GetHandle());


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
}
void Application::Run()
{
	MSG msg = { 0 };
	Load();
	while (msg.message != WM_QUIT)
	{
		static int count = -1;
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if(GlobalLinkData::_EnginePerformance)
				GlobalLinkData::_EnginePerformance->Reset();
		
			ENGINE_PERFORMANCE_TIMER_START("Application");


			if (m_InputEngine->GetKeyDown(KeyCode::LeftMouseButton))
			{
				auto window = JWindowManager::Find(GetFocus());
				if (window)
				{
					auto delta = m_InputEngine->GetMouseDeltaFromScreen();

					window->AddPosition((float)delta.x, (float)delta.y);
				}
	
			}
			if (m_InputEngine->GetKeyAsButton(KeyCode::A))
			{
				JWindowDesc desc;
				desc.name = "JWindow" + to_string(++count);
				desc.width = 800;
				desc.height = 600;
				desc.parent_hWnd = 0; // JWindowManager::Find("JWindow")->GetHandle();
				auto window = JWindowManager::Create(desc, 100, 100);
				RE::RenderEngine::CreateDxScreen(window->GetHandle(), desc.width, desc.height);
			}


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
			auto render_task = make_task([&] {
				m_RenderEngine->Update();
			});
			////엔진 업데이트

			structured_task_group _tasks;
			structured_task_group render_tasks;
			_tasks.run(input_task);
			_tasks.run(physics_task);
			_tasks.run(sound_task);
			_tasks.run(game_task);
			_tasks.wait();
			
			_tasks.run_and_wait(render_task);
			m_EngineTimer->Tick();


			if (m_InputEngine->GetKeyAsButton(KeyCode::Esc))
			{
				HWND hwnd = GetFocus();
				RE::RenderEngine::DestroyDxScreen(hwnd);
				JWindowManager::Destroy(hwnd);
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
