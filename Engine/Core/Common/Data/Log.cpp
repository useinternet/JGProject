#include<PCH.h>
#include"Log.h"
#include"Path.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include"Util/StringUtil.h"
REISTER_COMMONPATH
using namespace std;
namespace Common
{
	namespace Log
	{


		static unordered_map<string, LogStream> g_Logs;
		static ThreadSafeQueue<LogMessage> g_MsgQueue;
		void CreateLog(const string& GroupName, const string& logPath)
		{
			if (g_Logs.find(GroupName) != g_Logs.end())
				return;
			g_Logs[GroupName].path = logPath;
			g_Logs[GroupName].fout.open(logPath);
			g_Logs[GroupName].fout.close();
		}
		void LogPush(
			string filename,
			string funcname, 
			uint32_t lineNumber,
			const char* GroupName, 
			ELogLevel level, 
			const char* Contents, ...)
		{
			if (g_Logs.find(GroupName) == g_Logs.end())
			{
				CreateLog(GroupName, GLOBAL_PATH / "Engine/Log/" + GroupName + ".txt");
			}
			char buf[512] = { 0, };

			// 가변인자 처리
			{
				va_list ap;
				va_start(ap, Contents);
				vsprintf(buf, Contents, ap);
				va_end(ap);
			}

			g_Logs[GroupName].fout.open(g_Logs[GroupName].path, ios::app);
			string logMsg;

			uint32_t hashCode = (uint32_t)hash<thread::id>{}(this_thread::get_id());
				
			// 로그 메세지 처리
			{
				logMsg += "[ " + to_string(hashCode) + " ]";
				logMsg += "[ " + std::string(GroupName) + " ]";
				logMsg += "[ " + LogLevelToString(level) + " ]" + "\n";
				logMsg += "FileName : " + Util::Path_ExtractFileName(filename) + "\n";
				logMsg += "FuncName : " + funcname + "\n";
				logMsg += "Line     : " + std::to_string(lineNumber) + "\n";
				logMsg += "Contents : " + string(buf) + "\n";
			}
			// 메세지 큐에 담기
			{
				LogMessage msg = { std::string(GroupName), filename, funcname, lineNumber, level, hashCode, buf };
				g_MsgQueue.Push(msg);
			}

			g_Logs[GroupName].fout << logMsg;
			g_Logs[GroupName].fout.close();
		}
		ThreadSafeQueue<LogMessage>* GetLogAddress() {
			return &g_MsgQueue;
		}

		string LogLevelToString(ELogLevel level)
		{
			string levelStr;
			switch (level)
			{
			case Info:
				levelStr = "Info";
				break;
			case Default:
				levelStr = "Default";
				break;
			case Warning:
				levelStr = "Warning";
				break;
			case Error:
				levelStr = "Error";
				break;
			case Critical:
				levelStr = "Critical";
				break;
			}

			return levelStr;
		}
	}
}