
#include "pch.h"
#include "Plugin.h"

using namespace std;

Plugin::~Plugin()
{
	Free();

}

void Plugin::Load(const std::string& path)
{
	m_hModule = LoadLibraryA(path.c_str());
	
}

FARPROC Plugin::GetProcAddress(const std::string& name)
{
	return ::GetProcAddress(m_hModule, name.c_str());
}


void Plugin::Free()
{
	if (m_hModule)
	{
		FreeLibrary(m_hModule);
	}
}