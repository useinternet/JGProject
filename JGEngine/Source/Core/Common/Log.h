#pragma once
#include "Define.h"
#include "Abstract.h"
#include "String.h"
//#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
//#define SPDLOG_WCHAR_FILENAMES 
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace JG
{
	class Log : public GlobalSingleton<Log>
	{
		friend class Application;
	private:
		SharedPtr<spdlog::logger> smCoreLogger;
		SharedPtr<spdlog::logger> smClientLogger;
	public:
		inline static SharedPtr<spdlog::logger>& GetCoreLogger()  { return GetInstance().smCoreLogger; }
		inline static SharedPtr<spdlog::logger>& GetClientLogger() { return GetInstance().smClientLogger; }
	public:
		Log()
		{
			spdlog::set_pattern("%^[%T] %n: %v%$");
			smCoreLogger = spdlog::stdout_color_mt("Core");
			smCoreLogger->set_level(spdlog::level::trace);
			smClientLogger = spdlog::stdout_color_mt("App");
			smClientLogger->set_level(spdlog::level::trace);
		}
		~Log()
		{
			smCoreLogger.reset();
			smClientLogger.reset();
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
		//static auto Convert(const wchar_t* arg)
		//{
		//	return ws2s(arg);
		//}
		//static auto Convert(String& arg)
		//{
		//	return ws2s(arg);
		//}
		//static auto Convert(const String& arg)
		//{
		//	return ws2s(arg);
		//}
	};
}


//#if   _DEBUG
#define JG_CORE_TRACE(...)    ::JG::Log::TraceLog(::JG::Log::GetCoreLogger(), __VA_ARGS__);
#define JG_CORE_INFO(...)     ::JG::Log::InfoLog(::JG::Log::GetCoreLogger(), __VA_ARGS__);
#define JG_CORE_WARN(...)     ::JG::Log::WarnLog(::JG::Log::GetCoreLogger(), __VA_ARGS__);
#define JG_CORE_ERROR(...)    ::JG::Log::ErrorLog(::JG::Log::GetCoreLogger(), __VA_ARGS__);
#define JG_CORE_CRITICAL(...) ::JG::Log::CriticalLog(::JG::Log::GetCoreLogger(), __VA_ARGS__);
//#elif _RELEASE
//#define JG_CORE_TRACE(...)   
//#define JG_CORE_INFO(...)     
//#define JG_CORE_WARN(...)     
//#define JG_CORE_ERROR(...)    
//#define JG_CORE_CRITICAL(...) 
//#endif //  

#if   _DEBUG
#define JG_TRACE(...)   ::JG::Log::TraceLog(::JG::Log::GetClientLogger(), __VA_ARGS__);
#define JG_INFO(...)    ::JG::Log::InfoLog(::JG::Log::GetClientLogger(), __VA_ARGS__);
#define JG_WARN(...)    ::JG::Log::WarnLog(::JG::Log::GetClientLogger(), __VA_ARGS__);
#define JG_ERROR(...)   ::JG::Log::ErrorLog(::JG::Log::GetClientLogger(), __VA_ARGS__);
#define JG_CRITICAL(...)::JG::Log::CriticalLog(::JG::Log::GetClientLogger(), __VA_ARGS__);
#elif _RELEASE
#define JG_TRACE(...)   
#define JG_INFO(...)     
#define JG_WARN(...)     
#define JG_ERROR(...)    
#define JG_CRITICAL(...) 
#endif //  