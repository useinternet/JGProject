#pragma once
#include"Common.h"
#include"ThreadSafeQueue.h"

#ifdef _RELEASE
#define REGISTER_LOG(GroupName, LogPath) 	                      
#define DESTORY_LOG                                                
#define ENGINE_LOG(GroupName, LogLevel, Contents, ...)                  
#else

#define REGISTER_LOG(GroupName, LogPath) 	                      \
     Common::Log::CreateLog(GroupName, LogPath)                   \

#define DESTORY_LOG(GroupName)                                               \
    Common::Log::DestroyLog(GroupName)                                     \

#define ENGINE_LOG(GroupName, LogLevel, Contents, ...)                  \
	Common::Log::LogPush(__FILE__, __FUNCTION__, __LINE__, GroupName, LogLevel, Contents, __VA_ARGS__) \

#endif

namespace Common
{
	namespace Log
	{
		enum ELogLevel
		{
			Info = 0,
			Default,
			Warning,
			Error,
			Critical,
			LogLevel_Num
		};
		struct LogMessage
		{
			std::string groupName;
			ELogLevel   level;
			uint32_t    threadID;
			std::string str;
		};
		struct LogStream
		{
			std::string   path;
			std::ofstream fout;
		};

		void CreateLog(
			const std::string& GroupName,
			const std::string& logPath);

		void LogPush(
			std::string filename,
			std::string funcname,
			uint32_t lineNumber,
			const char*   GroupName,
			ELogLevel     level, 
			const char*   Contents,
			...);
		std::string LogLevelToString(ELogLevel level);
		ThreadSafeQueue<LogMessage>* GetLogAddress();
		typedef ThreadSafeQueue<LogMessage> EngineLog;
	}
}