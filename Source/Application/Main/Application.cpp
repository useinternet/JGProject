#include "pch.h"
#include "Application.h"
#include "CommonCore.h"

using namespace std;
using namespace Concurrency;
Application::Application() noexcept
{

}

Application::~Application() noexcept
{
	Destroy();


}

void Application::Run()
{
	Init();
	Start();
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Update();
	}
}

void Application::Init()
{

	m_MessageBus = make_unique<MessageBus>();
	m_EventManager = make_unique<EventManager>();
	m_WindowManager = make_unique<JWindowManager>();
	m_LogSystem = make_unique<LogSystem>();
	m_EngineTimer = make_unique<EngineTimer>();
	m_LogSystem->Create("engineLog.txt");
	/*
	SystemCore 받아오기
	*/
	std::string config_path = "none";
	LoadConfig(config_path);



	//JGLOG(LogLevel::Info, App, "테스트 로그", 1);
}

void Application::Start()
{
	static std::map<int, std::vector<SystemCore*>> start_sysCore;
	if (start_sysCore.empty())
	{
		for (auto& sys : m_SysCores)
		{
			start_sysCore[sys->GetDesc().start_priority].push_back(sys);
		}
	}


	for (auto& sys_pair : start_sysCore)
	{
		parallel_for((size_t)0, sys_pair.second.size(), [&](size_t i)
		{
			sys_pair.second[i]->Start();
		});
	}
	m_EngineTimer->Start();
}

void Application::Update()
{
	m_EngineTimer->Tick();
	{
		Message msg;
		msg.receiptID = GlobalSharedData::GetMessageBus()->GetID(GRAPHICS_ENGINE_MESSAGE_ID);
		msg.msgID = GraphicsMessage::Msg_BeginDraw;
		GlobalSharedData::GetMessageBus()->Send(msg);
	}



	static std::map<int, std::vector<SystemCore*>> update_sysCore;
	if (update_sysCore.empty())
	{
		for (auto& sys : m_SysCores)
		{
			update_sysCore[sys->GetDesc().update_priority].push_back(sys);
		}
	}

	for (auto& sys_pair : update_sysCore)
	{
		parallel_for((size_t)0, sys_pair.second.size(), [&](size_t i)
		{
			sys_pair.second[i]->Update();
		});
	}

	{
		Message msg;
		msg.receiptID = GlobalSharedData::GetMessageBus()->GetID(GRAPHICS_ENGINE_MESSAGE_ID);
		msg.msgID = GraphicsMessage::Msg_EndDraw;
		GlobalSharedData::GetMessageBus()->Send(msg);
	}

	GlobalSharedData::GetMessageBus()->Clear();
}

void Application::Destroy()
{
	static std::map<int, std::vector<SystemCore*>> destroy_sysCore;
	if (destroy_sysCore.empty())
	{
		for (auto& sys : m_SysCores)
		{
			destroy_sysCore[sys->GetDesc().destroy_priority].push_back(sys);
		}
	}

	for (auto& sys_pair : destroy_sysCore)
	{
		parallel_for((size_t)0, sys_pair.second.size(), [&](size_t i)
		{
			sys_pair.second[i]->Destroy();
	
		});
	}
	for (auto& sys_pair : destroy_sysCore)
	{
		parallel_for((size_t)0, sys_pair.second.size(), [&](size_t i)
		{
			delete sys_pair.second[i];
			sys_pair.second[i] = nullptr;
		});
	}
	m_SysCores.clear();


	m_MessageBus.reset();
	m_EventManager.reset();
	m_WindowManager.reset();
}

bool Application::LoadConfig(const std::string& path)
{
	GlobalSharedData sharedData;
	sharedData.msgBus = m_MessageBus.get();
	sharedData.eventManager = m_EventManager.get();
	sharedData.winManager = m_WindowManager.get();
	sharedData.logSystem = m_LogSystem.get();
	sharedData.engineTimer = m_EngineTimer.get();
	LinkSharedData(sharedData);


	AppConfig config;
	std::ifstream fin(config.core_link_path);
	if (!fin.is_open())
	{
		return false;
	}



	string buffer;
	while (!fin.eof())
	{
		fin >> buffer;
		unique_ptr<Plugin> plugin = make_unique<Plugin>();
		plugin->Load(buffer + ".dll");
		if (plugin->IsValid())
		{
			if (buffer == "GameFrameWork")
			{
				auto func = plugin->GetProcAddress("_TestFunc");
				int n = 0;
			}
			LINK_GLOBAL_SHARED_DATA(plugin);
			//((GlobalSharedDataLinkFunc)plugin->GetProcAddress("LinkGlobalSharedData"))(sharedData);

			SystemCore* core = ((SystemCoreLoadFunc)plugin->GetProcAddress("CreateSystemCore_" + buffer))();
			core->Awake();
			m_SysCores.push_back(core);


			m_SysPlugins.push_back(move(plugin));
		}
	}

	return true;
}
