

#include "String.h"
#include "Misc/Hash.h"

#include <cctype>
#include <codecvt>
#include <xhash>


PString::PString(const char* string)
{
	setString(string);
}

PString::PString(const wchar_t* string)
{
	setWString(string);
}

PString& PString::operator=(const char* string)
{
	setString(string);
	return *this;
}
PString& PString::operator=(const wchar_t* string)
{
	setWString(string);
	return *this;
}

PString& PString::Append(const PString& string)
{
	_rawString += string._rawString;
	updateHashCode();

	return *this;
}

PString& PString::Insert(const PString& string, uint64 pos)
{
	_rawString.insert(pos, string.GetRawString());
	updateHashCode();
	return *this;
}

void PString::SubString(PString& outString, uint64 startPos, uint64 length) const
{
	outString.Reset();
	outString.setString(_rawString.substr(startPos, length));
}

PString& PString::ReplaceAll(const PString& pattern, const PString& replace) 
{ 
	if (pattern.Empty() == true)
	{
		return *this;
	}
	uint64 pos = 0;
	uint64 offset = 0;

	while ((pos = _rawString.find(pattern.GetRawString(), offset)) != PRawString::npos)
	{
		_rawString.replace(_rawString.begin() + pos, _rawString.begin() + pos + pattern.GetRawString().size(), replace.GetRawString());
		offset = pos + replace._rawString.size();
	}

	updateHashCode();
	return *this;
}

uint64 PString::Find(const PString& pattern, uint64 offset, uint64 order) const
{
	uint64 cnt = 0;
	uint64 result = PRawString::npos;
	while (cnt < order)
	{
		uint64 pos = _rawString.find(pattern.GetRawString(), offset);
		if (pos == PRawString::npos)
		{
			break;
		}
		offset = pos + pattern.Length();
		result = pos;
		cnt += 1;
	}

	return result;
}

PList<PString> PString::Split(char delimiter) const
{
	PList<PString> result;
	std::stringstream ss(_rawString);


	PRawString tmp;

	while (std::getline(ss, tmp, delimiter)) {
		result.push_back(PString(tmp.c_str()));
	}

	return result;
}

uint64 PString::GetStringTableID() const
{
	return _stringCode;
}

uint64 PString::Size() const
{
	return _rawString.size();
}
uint64 PString::Length() const
{
	return _rawString.length();
}

bool PString::Empty() const
{
	return _rawString.empty();
}

bool PString::Equal(const PString& string) const
{
	return _rawString == string.GetRawString();
}

bool PString::StartWidth(const PString& pattern) const
{
	if (Length() < pattern.Length())
	{
		return false;
	}
	return std::strncmp(_rawString.c_str(), pattern.GetRawString().c_str(), pattern.Length()) == 0;
}

bool PString::EndWidth(const PString& pattern) const
{
	uint64 len = Length();
	uint64 patternLen = pattern.Length();

	if (len < patternLen)
	{
		return false;
	}

	PString endStr;
	SubString(endStr, len - patternLen, patternLen);

	return std::strncmp(endStr.GetRawString().c_str(), pattern.GetRawString().c_str(), patternLen) == 0;
}

int32 PString::ToInt(int32 base) const
{
	return std::stoi(_rawString, nullptr, base);
}

float PString::ToFloat() const
{
	return std::stof(_rawString);
}

double PString::ToDouble() const
{
	return std::stod(_rawString);
}

void PString::Reset()
{
	_rawString = "";
	_stringCode = STRING_TABLE_NULL_ID;
	//_rawWString = L"";
	//_rawWString = STRING_TABLE_NULL_ID;
}

const PRawString& PString::GetRawString() const
{
	return _rawString;
}

const PRawWString& PString::GetRawWString() const
{
	//updateRawWString();
	//return _rawWString;
	static PRawWString result = L"";
	return result;
}

void PString::setString(const PRawString& string)
{
	_rawString = string;
	updateHashCode();
}
void PString::setWString(const PRawWString& string)
{
	_rawString = ws2s(string);
	updateHashCode();
	updateRawWString();
}

void PString::updateRawWString() const
{
	//if (_stringCode == _wstringCode)
	//{
	//	return;
	//}
	//_wstringCode = _stringCode;
	//_rawWString  = s2ws(_rawString);
}

void PString::updateHashCode()
{
	_stringCode = PHash::Hash<PRawString>(_rawString);
}

PRawWString PString::s2ws(const PRawString& str) const
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

PRawString PString::ws2s(const PRawWString& wstr) const
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}