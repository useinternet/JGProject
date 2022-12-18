#include "PCH/PCH.h"
#include "StringTable.h"
#include "String/String.h"

GStringTable::GStringTable(int32 removeCountPerFrame, int32 stringInfoLifeFrameCount)
	: _removeCountPerFrame(removeCountPerFrame)
	, _stringInfoLifeFrameCount(stringInfoLifeFrameCount)
{}

GStringTable::~GStringTable() 
{
	Flush();
}

void GStringTable::Update()
{
	removeOldStringInfos(_removeCountPerFrame);
}

void GStringTable::Destroy()
{
	Flush();
}

void GStringTable::RegisterString(const PString& str, uint64* outID, std::weak_ptr<HAtomicInt32>* outRefCount)
{
	if (outID == nullptr || outRefCount == nullptr)
	{
		return;
	}

	uint64 hashCode = str.GetStringTableID();
	if (hashCode == NULL_ID)
	{
		return;
	}

	HStringInfo* pStringInfo = nullptr;
	{
		HLockGuard<HMutex> lock(_mutex);
		if (_stringInfoMap.find(hashCode) == _stringInfoMap.end())
		{
			HStringInfo info;
			info.ID   = hashCode;
			info.Str  = str.GetRawString();
			info.WStr = s2ws(info.Str);
			info.RefCount = std::make_shared<HAtomicInt32>();
			_stringInfoMap[hashCode] = std::move(info);
			_stringIDQueue.push(info.ID);
		}

		pStringInfo = &_stringInfoMap[hashCode];
	}


	if (pStringInfo != nullptr)
	{
		*outID = pStringInfo->ID;
		*outRefCount = pStringInfo->RefCount;
	}
}

bool GStringTable::FindString(uint64 ID, PString* outStr)  const
{
	if (outStr != nullptr)
	{
		*outStr = NULL_STR.c_str();
	}

	HRawString result;
	if (FindRawString(ID, &result) == true)
	{
		if (outStr != nullptr)
		{
			*outStr = result.c_str();
		}
		
		return true;
	}

	return false;
}
bool GStringTable::FindRawString(uint64 ID, HRawString* outStr) const
{
	if (outStr != nullptr)
	{
		*outStr = NULL_STR;
	}

	const HStringInfo* pInfo = findStringInfo(ID);
	if (pInfo == nullptr)
	{
		return false;
	}

	if (outStr != nullptr)
	{
		*outStr = pInfo->Str;
	}

	return true;
}
bool GStringTable::FindRawWString(uint64 ID, HRawWString* outStr) const
{
	if (outStr != nullptr)
	{
		*outStr = NULL_WSTR;
	}

	const HStringInfo* pInfo = findStringInfo(ID);
	if (pInfo == nullptr)
	{
		return false;
	}

	if (outStr != nullptr)
	{
		*outStr = pInfo->WStr;
	}

	return true;
}

void GStringTable::Flush()
{
	removeOldStringInfos(-1);
}

const GStringTable::HStringInfo* GStringTable::findStringInfo(uint64 id) const
{
	if (id == NULL_ID)
	{
		return nullptr;
	}

	HLockGuard<HMutex> lock(_mutex);

	auto iter = _stringInfoMap.find(id);
	if (iter == _stringInfoMap.end())
	{
		return nullptr;
	}

	return &(iter->second);
}

void GStringTable::removeOldStringInfos(int32 removeCountPerFrame)
{
	HLockGuard<HMutex> lock(_mutex);

	bool bIsFlush = false;

	if (removeCountPerFrame < 0)
	{
		bIsFlush = true;
		removeCountPerFrame = (int32)_stringInfoMap.size();
	}

	int32 tempCnt = 0;
	while (_stringIDQueue.empty() == false)
	{
		if (tempCnt >= removeCountPerFrame)
		{
			break;
		}

		uint64 ID = _stringIDQueue.front(); _stringIDQueue.pop();

		HStringInfo& infoRef  = _stringInfoMap[ID];
		uint64		 refCount = *infoRef.RefCount;
		if (refCount == 0)
		{
			infoRef.FrameCount += 1;

			if (bIsFlush == true || infoRef.FrameCount >= _stringInfoLifeFrameCount)
			{
				_stringInfoMap.erase(ID);
			}
		}
		else
		{
			infoRef.FrameCount = 0;
		}

		++tempCnt;
	}
}

HRawWString GStringTable::s2ws(const HRawString& str) const
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

HRawString GStringTable::ws2s(const HRawWString& wstr) const
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}