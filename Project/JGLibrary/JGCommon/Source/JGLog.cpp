#include"JGLog.h"
#include"JThreadManager.h"
#include<fstream>
using namespace std;
#ifdef _DEBUG
std::string m_LogLevel[4] = {
	"Info",
	"Warning", 
	"Error", 
	"Critical"
};
static ofstream lgFile;

void JGLog::InitLog(const std::string& logPath)
{
	lgFile.open("EngineCoreLog.log");
	lgFile.close();
}
void JGLog::Write(const ELogLevel level, const std::string& ClassName, const std::string& Exp)
{
	lgFile.open("EngineCoreLog.log", ios::app);
	lgFile << "[" << m_LogLevel[(int)level] << "] " << "[" << ClassName << "] " << " : " << Exp << endl;
	lgFile.close();
}
#endif