#pragma once
#include"common.h"




namespace JGPath
{
	class Path
	{
	private:
		std::string  m_Path;
		std::wstring m_Pathw;
	public:
		void SetPath(const std::string& path);
	public:
		Path(const std::string& path);
		std::string operator/(const std::string& path);
		std::wstring operator/(const std::wstring& path);
	};
	
}


