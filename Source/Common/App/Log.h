#pragma once

#include "GlobalSharedData.h"

#include<iostream>
#include<string>
#include<fstream>
#include <filesystem>
#include <thread>

#include <stdarg.h>
#include <sstream>


enum class LogLevel
{
	Debug,
	Info,
	Warning,
	Error,
	Fatal
};

class LogPiece
{
public:
	LogLevel level;
	std::thread::id threadID;
	std::string fileName;
	std::string funcName;

	std::string category;
	std::string contents;
};

class LogSystem
{
public:
	LogSystem();
	bool Create(const std::string& path);
	void Print(LogLevel logLevel, const std::wstring& fileName, const std::wstring& funcName,
		const std::wstring& category, const std::wstring& contents, ...);

	
	~LogSystem();
private:
	std::wstring LevelToString(LogLevel level);

private:
	std::string m_LogPath;
	std::mutex  m_LogMutex;
};




#define JGLOG(loglevel, category, contents, ...) \
 GlobalSharedData::GetLogSystem()->Print(loglevel, __FILEW__, __FUNCTIONW__, TT(#category), TT(contents), __VA_ARGS__ ); \
