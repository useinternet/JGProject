#include "pch.h"
#include "Log.h"

//#pragma comment(lib, "spdlog.lib")

std::shared_ptr<spdlog::logger> Log::sm_Logger;
std::string Log::sm_FileName;
void Log::Init(const std::string& loggerName, const std::string& filename)
{
	sm_FileName = filename;
	sm_Logger = spdlog::basic_logger_mt(loggerName, filename, true);
	spdlog::set_level(spdlog::level::trace);
	spdlog::set_pattern("[%l] [%t] %v");
	sm_Logger->flush_on(spdlog::level::trace);
}
void Log::RegisterLog(const std::shared_ptr<spdlog::logger>& logger, const std::string& filename)
{
	sm_FileName = filename;
	sm_Logger = logger;
	spdlog::register_logger(logger);
}
std::string Log::GetFileName()
{
	return sm_FileName;
}
std::shared_ptr<spdlog::logger> Log::GetLogger()
{
	return sm_Logger;
}