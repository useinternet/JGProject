#pragma once

#include "CoreDefines.h"
#include "CoreSystem.h"
#include "String/String.h"

class GStringTable : public GGlobalSystemInstance<GStringTable>
{
	const PRawString    NULL_STR  = "(null)";
	const PRawWString   NULL_WSTR = L"(null)";

	struct PStringInfo
	{
		uint64 ID = NULL_ID;
		uint64 FrameCount = 0;
		PRawString  Str;
		PRawWString WStr;
		std::unique_ptr<AtomicInt32> RefCount;

		PStringInfo() = default;
		PStringInfo(const PRawString& str, const PRawWString& wstr)
			: ID(NULL_ID)
			, Str(str)
			, WStr(wstr) {}
	};

	int32 _stringInfoLifeFrameCount;
	int32 _removeCountPerFrame;

	PHashMap<uint64, PStringInfo> _stringInfoMap;
	PQueue<uint64> _stringIDQueue;
	

	mutable PMutex _mutex;

public:
	GStringTable(int32 removeCountPerFrame = 1, int32 stringInfoLifeFrameCount = JG_INT32_MAX);
	virtual ~GStringTable() = default;

protected:
	virtual void Update() override;

public:
	void RegisterString(const PString& str, uint64* outID, AtomicInt32** outRefCount);

	bool FindString(uint64 ID, PString* outStr)  const;
	bool FindRawString(uint64 ID, PRawString* outStr) const;
	bool FindRawWString(uint64 ID, PRawWString* OutStr) const;

	void Flush();

private:
	const PStringInfo* findStringInfo(uint64 id) const;
	void removeOldStringInfos(int32 removeCountPerFrame);

	PRawWString s2ws(const PRawString& str) const;
	PRawString ws2s(const PRawWString& wstr) const;
};