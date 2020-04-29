#pragma once
#include <iostream>
#include <string>
#include <codecvt>
#define __CLASS__ GetClassNameByFullFuncName(__FUNCTION__)

inline std::string GetClassNameByFullFuncName(const char* fullFuncName)
{
	std::string fullFuncNameStr(fullFuncName);
	size_t pos = fullFuncNameStr.find_last_of("::");
	if (pos == std::string::npos)
	{
		return "";
	}
	return fullFuncNameStr.substr(0, pos - 1);
}
inline std::wstring s2ws(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

inline std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}
namespace std
{
	inline std::string replaceAll(const std::string& str, const std::string& pattern, const std::string& replace)
	{
		std::string result = str;
		std::string::size_type pos = 0;
		std::string::size_type offset = 0;
		while ((pos = result.find(pattern, offset)) != std::string::npos)
		{
			result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
			offset = pos + replace.size();
		}
		return result;
	}

}

