#include "pch.h"

#include "Log.h"

using namespace std;

LogSystem::LogSystem()
{


}

bool LogSystem::Create(const std::string& path)
{
	m_LogPath = path;
	std::locale::global(std::locale("Korean"));

	std::wofstream fout;
	fout.open(m_LogPath);
	if (!fout.is_open()) return false;


	fout.close();
	return true;
}

void LogSystem::Print(LogLevel logLevel,
	const std::wstring& fileName, const std::wstring& funcName, const std::wstring& category, const std::wstring& contents, ...)
{
	size_t len = contents.size();
	wstring buffer;
	buffer.resize(512);


	va_list vaList;
	va_start(vaList, contents);

	vswprintf_s(&buffer[0], 512, contents.data(), vaList);

	va_end(vaList);



	//
	len = wcslen(buffer.c_str());
	buffer.resize(len);

	std::wostringstream ostr;
	ostr << std::this_thread::get_id();
	wstring threadID = ostr.str();




	//wstring Log = TT("[") + LevelToString(logLevel) + TT("][") + threadID + TT("][") + fileName + TT("][") + funcName + TT("] : ") + buffer;
	wstring Log = TT("[") + LevelToString(logLevel) + TT("][") + threadID + TT("][") + funcName + TT("] : ") + buffer;


	{
		std::wofstream fout;
		fout.open(m_LogPath, ios::app);
		if (!fout.is_open()) return;
		lock_guard<mutex> lock(m_LogMutex);
		fout << Log << endl;
		fout.close();
	}

}

LogSystem::~LogSystem()
{


}

std::wstring LogSystem::LevelToString(LogLevel level)
{
	switch (level)
	{
	case LogLevel::Debug:   return TT("Debug");
	case LogLevel::Info:    return TT("Info");
	case LogLevel::Warning: return TT("Warning");
	case LogLevel::Error:   return TT("Error");
	case LogLevel::Fatal:   return TT("Fatal");
	default: return TT("");
	}
}
