#pragma once
#include"Common.h"


namespace Common
{
	namespace Util
	{
		inline std::wstring AnsiToWString(const std::string& str)
		{
			WCHAR buffer[512];
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
			return std::wstring(buffer);
		}
		inline std::string AnsiToString(const std::wstring& str)
		{
			std::string cstr;
			cstr.insert(cstr.end(), str.begin(), str.end());
			return cstr;
		}
		inline bool IsExsitString(const std::string& OriginStr, const std::string& str)
		{
			return OriginStr.find(str) != std::string::npos;
		}
		inline bool IsExsitString(const std::wstring& OriginStr, const std::wstring& str)
		{
			return OriginStr.find(str) != std::wstring::npos;
		}
		inline std::string ExtractString(const std::string& OriginStr, const std::string& extractStr)
		{
			size_t len = extractStr.length();
			if (IsExsitString(OriginStr, extractStr))
			{
				size_t pos = OriginStr.find(extractStr);
				return OriginStr.substr(pos, len);
			}
			else
			{
				return "";
			}
		}
		inline std::string Path_ExtractFileName(const std::string& Path)
		{
			if (IsExsitString(Path, "\\"))
			{
				size_t pos = Path.rfind("\\");
				return Path.substr(pos + 1);
			}
			else if (IsExsitString(Path, "/"))
			{
				size_t pos = Path.rfind("/");
				return Path.substr(pos + 1);
			}
			else
			{
				return "";
			}
		}
		inline std::string Path_ExtractFileName_NoFormat(const std::string& path)
		{
			std::string result = Path_ExtractFileName(path);
			if (IsExsitString(result, "."))
			{
				size_t pos = result.rfind(".");
				return result.substr(0, pos);
			}
			else
				return result;
		}
		inline std::string Path_ExtractFileFormat(const std::string& path)
		{
			std::string result = Path_ExtractFileName(path);
			if (IsExsitString(result, "."))
			{
				size_t pos = result.rfind(".");
				return result.substr(pos);
			}
			else
				return result;
		}
		inline std::wstring ExtractString(const std::wstring& OriginStr, const std::wstring& extractStr)
		{
			size_t len = extractStr.length();
			if (IsExsitString(OriginStr, extractStr))
			{
				size_t pos = OriginStr.find(extractStr);
				return OriginStr.substr(pos, len);
			}
			else
			{
				return L"";
			}
		}
		inline std::wstring Path_ExtractFileName(const std::wstring& Path)
		{
			if (IsExsitString(Path, L"\\"))
			{
				size_t pos = Path.rfind(L"\\");
				return Path.substr(pos + 1);
			}
			else if (IsExsitString(Path, L"/"))
			{
				size_t pos = Path.rfind(L"/");
				return Path.substr(pos + 1);
			}
			else
			{
				return L"";
			}
		}
		inline std::wstring Path_ExtractFileName_NoFormat(const std::wstring& path)
		{
			std::wstring result = Path_ExtractFileName(path);
			if (IsExsitString(result, L"."))
			{
				size_t pos = result.rfind(L".");
				return result.substr(0, pos);
			}
			else
				return result;
		}
		inline std::wstring Path_ExtractFileFormat(const std::wstring& path)
		{
			std::wstring result = Path_ExtractFileName(path);
			if (IsExsitString(result, L"."))
			{
				size_t pos = result.rfind(L".");
				return result.substr(pos);
			}
			else
				return result;
		}
	}
}
