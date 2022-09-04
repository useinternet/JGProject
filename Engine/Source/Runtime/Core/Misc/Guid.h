#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"

// 나중에 구현
class PGuid : public IMemoryObject
{
	uint64 _data1;
	uint16 _data2;
	uint16 _data3;
	uint8  _data4[8];


public:
	PGuid();
	virtual ~PGuid() = default;
	//bool IsValid() const;



};