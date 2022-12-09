#pragma once


#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "Name.h"
#include <iostream>


#pragma warning(disable : 4996)

class PString : public IMemoryObject
{
public:
	static const uint64 NPOS = -1;

private:
	uint64	    _stringCode = NULL_ID;
	HRawString  _rawString;

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

		result._rawString.resize(len - 1); // '\0' 맨끝 제거
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

	template<>
	static auto convert(const PName& arg)
	{
		return arg.ToString().GetCStr();
	}
public:
	PString() = default;
	PString(char inChar);
	PString(const char* string);

	virtual ~PString();

public: // -- operation --
	PString& operator=(const PString& string) = default;
	PString& operator=(const char inChar);
	PString& operator=(const char* string);
	PString  operator+(const PString& string) const;
	PString& operator+=(const PString& string);

	bool operator==(const PString& string) const;
	bool operator!=(const PString& string) const;

	PString operator/(const PString& string) const;

	char& operator[](uint64 index);
	const char& operator[](uint64 index) const;
public:
	PString& Append(const PString& string);
	PString& AppendLine(const PString& string);
	PString& Insert(const PString& string, uint64 pos);
	PString& Remove(uint64 startPos, uint64 len);
	PString& Trim();

	void SubString(PString* outString, uint64 startPos, uint64 length = PString::NPOS) const;

	PString& ReplaceAll(const PString& pattern, const PString& replace);

	uint64 Find(const PString& pattern, uint64 offset = 0, uint64 order = 1) const;
	uint64 FindLastOf(const PString& pattern, uint64 offset = PString::NPOS, uint64 order = 1) const;

	HList<PString> Split(char delimiter) const;

	uint64 GetStringTableID() const;
	uint64 Size() const;
	uint64 Length() const;
	bool Empty() const;
	bool Equal(const PString& string) const;
	bool StartWidth(const PString& pattern) const;
	bool EndWidth(const PString& pattern) const;
	bool Contains(const PString& pattern) const;

	int32  ToInt(int32 base = 0) const;
	float  ToFloat() const;
	double ToDouble() const;

	void Reset();
	void Resize(uint64 size);
public:
	char GetChar(uint64 index) const;
	const char* GetCStr() const;
	const HRawString& GetRawString() const;
	HRawWString GetRawWString() const;
	void GetRawWString(HRawWString* outWStr) const;
private:
	void setString(const HRawString& string);
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