#pragma once
#pragma warning(disable : 26495 26498 26451 6285 4819 26437 26800)

#include "CoreDefines.h"
#include "CoreSystem.h"
#include "Memory/Memory.h"
#include "String/String.h"
#include "FileIO/FileHelper.h"
#include "spdlog/spdlog.h"

enum class ELogLevel
{
	Debug,
	Info,
	Warning,
	Error,
	Critical,
};

class GLogGlobalSystem : public GGlobalSystemInstance<GLogGlobalSystem>
{
	mutable std::shared_ptr<spdlog::logger> _logger;

public:
	GLogGlobalSystem();
	virtual ~GLogGlobalSystem();

public:
	template<class ...T>
	void AddLog(const PString& category, ELogLevel loglevel, T&& ...args) const
	{
		PString log = PString::Format("[%s]: %s", category.GetRawString().c_str(), args...);
		switch (loglevel)
		{
		case ELogLevel::Debug:    _logger->trace(log.GetRawString());	 break;
		case ELogLevel::Info:     _logger->info(log.GetRawString());	 break;
		case ELogLevel::Warning:  _logger->warn(log.GetRawString());	 break;
		case ELogLevel::Error:	  _logger->error(log.GetRawString());	 break;
		case ELogLevel::Critical: _logger->critical(log.GetRawString()); break;
		}
	}
};


#define JG_LOG(Category, LogLevel, ...) ::GLogGlobalSystem::GetInstance().AddLog(#Category, LogLevel, __VA_ARGS__);