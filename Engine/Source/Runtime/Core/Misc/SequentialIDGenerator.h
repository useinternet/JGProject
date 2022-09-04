#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"


class PSequentialIDGenerator : public IMemoryObject
{
	uint64 _idOffset;
	HQueue<uint64> _pendingIDQueue;
	HHashSet<uint64> _idPool;

public:
	PSequentialIDGenerator();
	virtual ~PSequentialIDGenerator() = default;

public:
	uint64 NewID();
	bool RetrieveID(uint64 id);
	void Reset();
};
