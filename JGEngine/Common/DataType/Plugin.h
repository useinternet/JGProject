#pragma once

#include <Windows.h>
#include <string>
class Plugin
{
	HMODULE m_hModule;
public:
	~Plugin();
public:
	void Load(const std::string& path);
	FARPROC GetProcAddress(const std::string& name);
	bool IsValid() const {
		return m_hModule != nullptr;
	}
private:
	void Free();
};