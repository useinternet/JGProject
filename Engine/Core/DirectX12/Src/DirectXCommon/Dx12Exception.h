#pragma once
#include<string>
#include<comdef.h>
#include<Util/StringUtil.h>

class DxException : public std::exception
{
public:
	DxException() = default;
	DxException(long hr, const std::wstring& functionName, const std::string& filename, int lineNumber);

	std::string ToString()const {
		// Get the string description of the error code.www
		_com_error err(ErrorCode);
		std::string msg = Common::Util::AnsiToString(err.ErrorMessage());
		
		return FunctionName + " failed in " + Filename + "; line " + std::to_string(LineNumber) + "; error: " + msg;
	}

	long ErrorCode = 0;
	std::string FunctionName;
	std::string Filename;
	int LineNumber = -1;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, __FILE__, __LINE__); } \
}
#endif