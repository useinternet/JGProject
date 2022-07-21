#pragma once


#include "CoreDefines.h"
#include <iostream>



#pragma warning(disable : 4996)

using PRawString  = std::string;
using PRawWString = std::wstring;

#define STRING_TABLE_NULL_ID -1

class PString
{
private:
	uint64	   _stringCode = STRING_TABLE_NULL_ID;
	PRawString  _rawString;
public:
	template <class ...Args>
	static PString Format(const PString& string, Args&& ... args)
	{
		if (string.Empty() == true)
		{
			return PString();
		}

		uint64 len = snprintf(nullptr, 0, string.GetRawString().c_str(), args ...) + 1; // Extra space for '\0'
		if (len <= 0)
		{
			return PString();
		}

		PString result;
		result._rawString.resize(len);

		snprintf(result._rawString.data(), len, string.GetRawString().c_str(), args ...);
		return result;
	}
	static PString ReplaceAll(const PString& message, const PString& pattern, const PString& replace)
	{
		PString result = message;
		return result.ReplaceAll(pattern, replace);
	}
public:
	PString() = default;
	PString(const char* string);
	PString(const wchar_t* string);

	virtual ~PString() = default;

public: // -- operation --
	PString& operator=(const PString& string) = default;
	PString& operator=(const char* string);
	PString& operator=(const wchar_t* string);

public:
	PString& Append(const PString& string);
	PString& Insert(const PString& string, uint64 pos);

	void SubString(PString& outString, uint64 startPos, uint64 length) const;

	PString& ReplaceAll(const PString& pattern, const PString& replace);

	uint64 Find(const PString& pattern, uint64 offset, uint64 order = 1) const;

	PList<PString> Split(char delimiter) const;

	uint64 GetStringTableID() const;
	uint64 Size() const;
	uint64 Length() const;
	bool Empty() const;
	bool Equal(const PString& string) const;
	bool StartWidth(const PString& pattern) const;
	bool EndWidth(const PString& pattern) const;

	int32  ToInt(int32 base = 0) const;
	float  ToFloat() const;
	double ToDouble() const;

	void Reset();

public:
	const PRawString& GetRawString() const;
	const PRawWString& GetRawWString() const;

private:
	void setString(const PRawString& string);
	void setWString(const PRawWString& string);
	void updateRawWString() const;
	void updateHashCode();
	PRawWString s2ws(const PRawString& str) const;
	PRawString ws2s(const PRawWString& wstr) const;
};