#pragma once


#include "CoreDefines.h"
#include "Memory/Memory.h"
#include <iostream>


#pragma warning(disable : 4996)

class PString : public IMemoryObject
{
public:
	static const uint64 NPOS = -1;

private:
	uint64	    _stringCode = NULL_ID;
	PRawString  _rawString;

public:
	template <class ...Args>
	static PString Format(const PString& string, Args&& ... args)
	{
		if (string.Empty() == true)
		{
			return PString();
		}

		uint64 len = (uint64)snprintf(nullptr, 0, string.GetRawString().c_str(), convert(args) ...) + 1; // Extra space for '\0'
		if (len <= 0)
		{
			return PString();
		}

		PString result;
		result._rawString.resize(len);

		snprintf(result._rawString.data(), len, string.GetRawString().c_str(), convert(args) ...);
		return result;
	}

	static PString ReplaceAll(const PString& message, const PString& pattern, const PString& replace)
	{
		PString result = message;
		return result.ReplaceAll(pattern, replace);
	}

private:
	template<class T>
	static auto convert(const T& arg)
	{
		return arg;
	}
	template<>
	static auto convert(const PString& arg)
	{
		return arg.GetCStr();
	}
public:
	PString() = default;
	PString(char inChar);
	PString(const char* string);
	PString(const wchar_t* string);

	virtual ~PString() = default;

public: // -- operation --
	PString& operator=(const PString& string) = default;
	PString& operator=(const char inChar);
	PString& operator=(const char* string);
	PString& operator=(const wchar_t* string);

	bool operator==(const PString& string) const;
	bool operator!=(const PString& string) const;

	char& operator[](uint64 index);
	const char& operator[](uint64 index) const;
public:
	PString& Append(const PString& string);
	PString& AppendLine(const PString& string);
	PString& Insert(const PString& string, uint64 pos);

	void SubString(PString& outString, uint64 startPos, uint64 length = PString::NPOS) const;

	PString& ReplaceAll(const PString& pattern, const PString& replace);

	uint64 Find(const PString& pattern, uint64 offset = 0, uint64 order = 1) const;
	uint64 FindLastOf(const PString& pattern, uint64 offset = PString::NPOS, uint64 order = 1) const;

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
	void Resize(uint64 size);
public:
	char GetChar(uint64 index) const;
	const char* GetCStr() const;
	const PRawString& GetRawString() const;
	const PRawWString& GetRawWString() const;

private:
	void setString(const PRawString& string);
	void setWString(const PRawWString& string);
	void updateHashCode();
};

namespace std {
	template <>
	struct hash<PString>
	{
		std::size_t operator()(const PString& k) const noexcept
		{
			return k.GetStringTableID();
		}
	};
}