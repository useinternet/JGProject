#pragma once

#include <Windows.h>
#include <string>
#include <memory>

#include "JE.h"
#include "DataType/Abstract/NoneCopy.h"

namespace RE
{
	class RenderEngine;
}
namespace IE
{
	class InputEngine;
}
namespace PE
{
	class PhysicsEngine;
}
namespace SE
{
	class SoundEngine;
}
namespace GFW
{
	class Game;
}
class JGUI;
enum class JGENGINE_API EApplicationMode
{
	Editor,
	GamePlay
};
class JGENGINE_API Application : public NoneCopy
{
public:
	static const GlobalLinkStream& GetLinkStream();
public:
	Application() = default;
	Application(const std::wstring& name, EApplicationMode mode);
	virtual ~Application();
	bool Init();
	void Load();
	void Run();
	void OnEvent(Event& e);
private:
	EApplicationMode m_AppMode;
	std::wstring     m_AppName;


	std::shared_ptr<IE::InputEngine>   m_InputEngine;
	std::shared_ptr<PE::PhysicsEngine> m_PhysicsEngine;
	std::shared_ptr<SE::SoundEngine>   m_SoundEngine;
	std::shared_ptr<RE::RenderEngine>  m_RenderEngine;
	std::shared_ptr<JGUI>              m_GUI;
	std::shared_ptr<GFW::Game>         m_Game;
	std::shared_ptr<EventManager>      m_EventManager;
	std::shared_ptr<EngineTimer>       m_EngineTimer;
	std::shared_ptr<EngineConfig>      m_EngineConfig;
	std::shared_ptr<EnginePerformance> m_Performance;
	bool m_IsInit;
	std::mutex m_EventMutex;
};