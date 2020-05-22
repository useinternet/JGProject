#pragma once

#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

class Log
{
	Log() = delete;
	~Log() = delete;
public:
	static void Init(
		const std::string& loggerName = "JGEngine",
		const std::string& filename = "enginelog.txt");
	static void RegisterLog(
		const std::shared_ptr<spdlog::logger>& logger, const std::string& filename);
	static std::string GetFileName();
	static std::shared_ptr<spdlog::logger> GetLogger();
private:
	static std::shared_ptr<spdlog::logger> sm_Logger;
	static std::string sm_FileName;
};


#define LOG(level, affiliation, str, ... ) Log::GetLogger()->##level("[" + std::string(#affiliation) + "] : " + std::string(str), __VA_ARGS__)

#define LOG_TRACE(affiliation, str, ...) Log::GetLogger()->trace("[" + std::string(#affiliation) + "] : " + std::string(str), __VA_ARGS__)
#define LOG_INFO(affiliation, str,...)  Log::GetLogger()->info(("[" + std::string(#affiliation) + "] : " + std::string(str), __VA_ARGS__)
#define LOG_WARN(affiliation, str,...)  Log::GetLogger()->warn(("[" + std::string(#affiliation) + "] : " + std::string(str), __VA_ARGS__)
#define LOG_ERROR(affiliation, str,...) Log::GetLogger()->error(("[" + std::string(#affiliation) + "] : " + std::string(str), __VA_ARGS__)
#define LOG_FATAL(affiliation, str,...) Log::GetLogger()->critical(("[" + std::string(#affiliation) + "] : " + std::string(str), __VA_ARGS__)



#define ENGINE_LOG_TRACE(str, ...) Log::GetLogger()->trace("[Engine] : " + std::string(str), __VA_ARGS__)
#define ENGINE_LOG_INFO(str,...)  Log::GetLogger()->info("[Engine] : " + std::string(str), __VA_ARGS__)
#define ENGINE_LOG_WARN(str,...)  Log::GetLogger()->warn("[Engine] : " + std::string(str), __VA_ARGS__)
#define ENGINE_LOG_ERROR(str,...) Log::GetLogger()->error("[Engine] : " + std::string(str), __VA_ARGS__)
#define ENGINE_LOG_FATAL(str,...) Log::GetLogger()->critical("[Engine] : " + std::string(str), __VA_ARGS__)

#define IE_LOG_TRACE(str,...) Log::GetLogger()->trace("[Input] : " + std::string(str), __VA_ARGS__)
#define IE_LOG_INFO(str,...)  Log::GetLogger()->info("[Input] : " + std::string(str), __VA_ARGS__)
#define IE_LOG_WARN(str,...)  Log::GetLogger()->warn("[Input] : " + std::string(str), __VA_ARGS__)
#define IE_LOG_ERROR(str,...) Log::GetLogger()->error("[Input] : " + std::string(str), __VA_ARGS__)
#define IE_LOG_FATAL(str,...) Log::GetLogger()->critical("[Input] : " + std::string(str), __VA_ARGS__)

#define PE_LOG_TRACE(str,...) Log::GetLogger()->trace("[Physics] : " + std::string(str), __VA_ARGS__)
#define PE_LOG_INFO(str,...)  Log::GetLogger()->info("[Physics] : " + std::string(str), __VA_ARGS__)
#define PE_LOG_WARN(str,...)  Log::GetLogger()->warn("[Physics] : " + std::string(str), __VA_ARGS__)
#define PE_LOG_ERROR(str,...) Log::GetLogger()->error("[Physics] : " + std::string(str), __VA_ARGS__)
#define PE_LOG_FATAL(str,...) Log::GetLogger()->critical("[Physics] : " + std::string(str), __VA_ARGS__)

#define RE_LOG_TRACE(str,...) Log::GetLogger()->trace("[Rendering] : " + std::string(str), __VA_ARGS__)
#define RE_LOG_INFO(str,...)  Log::GetLogger()->info("[Rendering] : " + std::string(str), __VA_ARGS__)
#define RE_LOG_WARN(str,...)  Log::GetLogger()->warn("[Rendering] : " + std::string(str), __VA_ARGS__)
#define RE_LOG_ERROR(str,...) Log::GetLogger()->error("[Rendering] : " + std::string(str), __VA_ARGS__)
#define RE_LOG_FATAL(str,...) Log::GetLogger()->critical("[Rendering] : " + std::string(str), __VA_ARGS__)

#define SE_LOG_TRACE(str,...) Log::GetLogger()->trace("[Sound] : " + std::string(str), __VA_ARGS__)
#define SE_LOG_INFO(str,...)  Log::GetLogger()->info("[Sound] : " + std::string(str), __VA_ARGS__)
#define SE_LOG_WARN(str,...)  Log::GetLogger()->warn("[Sound] : " + std::string(str), __VA_ARGS__)
#define SE_LOG_ERROR(str,...) Log::GetLogger()->error("[Sound] : " + std::string(str), __VA_ARGS__)
#define SE_LOG_FATAL(str,...) Log::GetLogger()->critical("[Sound] : " + std::string(str), __VA_ARGS__)

#define GFW_LOG_TRACE(str,...) Log::GetLogger()->trace("[GameFrameWork] : " + std::string(str), __VA_ARGS__)
#define GFW_LOG_INFO(str,...)  Log::GetLogger()->info("[GameFrameWork] : " + std::string(str), __VA_ARGS__)
#define GFW_LOG_WARN(str,...)  Log::GetLogger()->warn("[GameFrameWork] : " + std::string(str), __VA_ARGS__)
#define GFW_LOG_ERROR(str,...) Log::GetLogger()->error("[GameFrameWork] : " + std::string(str), __VA_ARGS__)
#define GFW_LOG_FATAL(str,...) Log::GetLogger()->critical("[GameFrameWork] : " + std::string(str), __VA_ARGS__)

