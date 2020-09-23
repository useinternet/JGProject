#pragma once

#pragma warning(disable : 4251 4275 26495 26451 6387 26439 26498)

#ifdef APPLICATION_EXPORT

#define APP_API __declspec(dllexport)

#else

#define APP_API __declspec(dllimport)

#endif


#include "CommonCore.h"
#include <memory>
#include <vector>
#include <map>
struct AppConfig
{
	std::string core_link_path = "../Source/Config/core_link.ini";
};
class APP_API Application
{

public:
	Application() noexcept;
	~Application() noexcept;
public:
	void Run();
private:
	void Init();
	void Start();
	void Update();
	void Destroy();
private:
	bool LoadConfig(const std::string& path);
private:
	// MessageBus
	std::unique_ptr<MessageBus>     m_MessageBus;
	std::unique_ptr<EventManager>   m_EventManager;
	std::unique_ptr<JWindowManager> m_WindowManager;
	std::unique_ptr<LogSystem>      m_LogSystem;
	std::unique_ptr<EngineTimer>    m_EngineTimer;
	std::unique_ptr<AssetManager>   m_AssetManager;
	// SystemCore
	std::vector<SystemCore*> m_SysCores;
	std::vector<std::unique_ptr<Plugin>> m_SysPlugins;
};