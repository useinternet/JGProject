#include "PCH/PCH.h"
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

GLogGlobalSystem::GLogGlobalSystem()
{
	HFileHelper::WriteAllText("jg_log.txt", "");

	spdlog::set_pattern("%^[:%t:][%l]%v%$");
	_logger = spdlog::basic_logger_mt("JGLog", "jg_log.txt");


	_logger->set_level(spdlog::level::trace);
	_logger->flush_on(spdlog::level::trace);

	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_pattern("%^[:%t:][%l]%v%$");
	_logger->sinks().push_back(consoleSink);
}

GLogGlobalSystem::~GLogGlobalSystem()
{
	_logger.reset();
	spdlog::shutdown();
}


