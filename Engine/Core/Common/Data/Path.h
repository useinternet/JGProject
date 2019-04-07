#pragma once
#include"Common.h"
#define REISTER_COMMONPATH \
namespace Common \
{   \
   extern SharePath g_CommonPath; \
} \

#define GLOBAL_PATH Common::g_CommonPath

namespace Common
{
	class SharePath
	{
#ifdef _DEVELOP_DEBUG
		std::wstring path = L"../../../";
		std::string cpath = "../../../";
#endif
#ifdef _DEVELOP_RELEASE
		std::wstring path = L"../../../";
		std::string cpath = "../../../";
#endif
#ifdef _RELEASE
		std::wstring path = L"./";
		std::string cpath = "./";
#endif
	public:
		std::wstring operator/(const std::wstring& path)
		{
			std::wstring str;
			str.insert(str.end(), this->path.begin(), this->path.end());

			return str + path;
		}
		std::string operator/(const std::string& path)
		{
			std::string str;
			str.insert(str.end(), this->path.begin(), this->path.end());

			return str + path;
		}
		std::wstring operator/(const wchar_t* path)
		{
			std::wstring str;
			str.insert(str.end(), this->path.begin(), this->path.end());

			return str + path;
		}
		std::string operator/(const char* path)
		{
			std::string str;
			str.insert(str.end(), this->path.begin(), this->path.end());

			return str + path;
		}
	};
}