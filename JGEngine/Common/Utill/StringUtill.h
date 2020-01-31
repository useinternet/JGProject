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