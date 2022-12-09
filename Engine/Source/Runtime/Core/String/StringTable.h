#pragma once

#include "CoreDefines.h"
#include "CoreSystem.h"


class PString;

class GStringTable : public GGlobalSystemInstance<GStringTable>
{
	const HRawString    NULL_STR  = "(null)";
	const HRawWString   NULL_WSTR = L"(null)";

	struct HStringInfo
	{
		uint64 ID = NULL_ID;
		uint64 FrameCount = 0;
		HRawString  Str;
		HRawWString WStr;
		std::shared_ptr<HAtomicInt32> RefCount;

		HStringInfo() = default;
		HStringInfo(const HRawString& str, const HRawWString& wstr)
			: ID(NULL_ID)
			, Str(str)
			, WStr(wstr) {}
	};

	int32 _stringInfoLifeFrameCount;
	int32 _removeCountPerFrame;

	HHashMap<uint64, HStringInfo> _stringInfoMap;
	HQueue<uint64> _stringIDQueue;
	

	mutable HMutex _mutex;

public:
	GStringTable(int32 removeCountPerFrame = 1, int32 stringInfoLifeFrameCount = JG_INT32_MAX);
	virtual ~GStringTable();

protected:
	virtual void Update() override;
	virtual void Destroy() override;
public:
	void RegisterString(const PString& str, uint64* outID, std::weak_ptr<HAtomicInt32>* outRefCount);

	bool FindString(uint64 ID, PString* outStr)  const;
	bool FindRawString(uint64 ID, HRawString* outStr) const;
	bool FindRawWString(uint64 ID, HRawWString* OutStr) const;

	void Flush();

private:
	const HStringInfo* findStringInfo(uint64 id) const;
	void removeOldStringInfos(int32 removeCountPerFrame);

	HRawWString s2ws(const HRawString& str) const;
	HRawString ws2s(const HRawWString& wstr) const;
};