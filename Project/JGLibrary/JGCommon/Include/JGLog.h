#pragma once
#include<string>
enum ELogLevel
{
	log_Info     = 0,
	log_Warning  = 1,
	log_Error    = 2,
	log_Critical = 3,
};
#ifdef _DEBUG

namespace JGLog
{
    void InitLog(const std::string& logPath);
	void Write(const ELogLevel level, const std::string& ClassName, const std::string& Exp);
}
#endif

#ifdef _DEBUG
#define JGLOGINIT(logPath) JGLog::InitLog(logPath);
#else
#define JGLOGINIT(logPath)
#endif

#ifdef _DEBUG
#define JGLOG(level, Scope , Exp) JGLog::Write(level,Scope,Exp);
#else
#define JGLOG(level, Scope, Exp)
#endif