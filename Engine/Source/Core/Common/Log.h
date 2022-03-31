#pragma once
#include "Define.h"
#include "Abstract.h"
#include "String.h"
#include "Class/Global/GlobalSingletonManager.h"
//#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
//#define SPDLOG_WCHAR_FILENAMES 
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
namespace JG
{
	class Log : public IGlobalSingleton<Log>
	{
	private:
		SharedPtr<spdlog::logger> smLogger;
	public:
		inline static SharedPtr<spdlog::logger>& GetLogger()  { return GetInstance().smLogger; }
		inline static String GetLogFile() { return "jg_log.txt"; }
	public:
		Log()
		{
			FileHelper::WriteAllText(GetLogFile(), "");
			spdlog::set_pattern("%^[:%t:][%l]%v%$");
			smLogger = spdlog::basic_logger_mt("JGLog", GetLogFile());


			smLogger->set_level(spdlog::level::trace);
			smLogger->flush_on(spdlog::level::trace);

			auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			consoleSink->set_pattern("%^[:%t:][%l]%v%$");
			smLogger->sinks().push_back(consoleSink);
		}
		virtual ~Log()
		{
			smLogger.reset();
			spdlog::shutdown();
		}
	public:
		template<class ...T>
		inline static void TraceLog(SharedPtr<spdlog::logger> logger, T...args)
		{
			logger->trace(Convert(args)...);
		}
		template<class ...T>
		inline static void InfoLog(SharedPtr<spdlog::logger> logger, T...args)
		{
			logger->info(Convert(args)...);
		}
		template<class ...T>
		inline static void WarnLog(SharedPtr<spdlog::logger> logger, T...args)
		{
			logger->warn(Convert(args)...);
		}
		template<class ...T>
		inline static void ErrorLog(SharedPtr<spdlog::logger> logger, T...args)
		{
			logger->error(Convert(args)...);
		}
		template<class ...T>
		inline static void CriticalLog(SharedPtr<spdlog::logger> logger, T...args)
		{
			logger->critical(Convert(args)...);
		}
	private:
		template<class T>
		static auto Convert(T&& arg)
		{
			return std::forward<T>(arg);
		}
	};
}


#if   _DEBUG
#define JG_LOG_TRACE(...)    ::JG::Log::TraceLog(::JG::Log::GetLogger(), __VA_ARGS__);
#define JG_LOG_INFO(...)     ::JG::Log::InfoLog(::JG::Log::GetLogger(), __VA_ARGS__);
#define JG_LOG_WARN(...)     ::JG::Log::WarnLog(::JG::Log::GetLogger(), __VA_ARGS__);
#define JG_LOG_ERROR(...)    ::JG::Log::ErrorLog(::JG::Log::GetLogger(), __VA_ARGS__);
#elif _RELEASE
#define JG_LOG_TRACE(...)    ::JG::Log::TraceLog(::JG::Log::GetLogger(), __VA_ARGS__);
#define JG_LOG_INFO(...)     ::JG::Log::InfoLog(::JG::Log::GetLogger(), __VA_ARGS__);
#define JG_LOG_WARN(...)     ::JG::Log::WarnLog(::JG::Log::GetLogger(), __VA_ARGS__);
#define JG_LOG_ERROR(...)    ::JG::Log::ErrorLog(::JG::Log::GetLogger(), __VA_ARGS__); 
#endif //  
 