#pragma once
#include <string>
#include <codecvt>



namespace JG
{
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

	inline String ReplaceAll(const String& message, const String& pattern, const String& replace)
	{
		String result = message;
		String::size_type pos = 0;
		String::size_type offset = 0;

		while ((pos = result.find(pattern, offset)) != String::npos)
		{
			result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
			offset = pos + replace.size();
		}
		return result;
	}

	inline String CombinePath(const String& dest, const String& src)
	{
		String result = "";
		u64 lastIndex = dest.length() - 1;
		if (src[0] != '/' && dest[lastIndex] != '/')
		{
			result = dest + '/' + src;
		}
		else if (src[0] == '/' && dest[lastIndex] == '/')
		{
			result = dest.substr(0, lastIndex) + src;
		}
		else
		{
			result = dest + src;
		}
		return result;
	}

}