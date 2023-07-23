#pragma once
#include "Memory/Memory.h"
#include "Object/JGType.h"

class PPayload : public IMemoryObject
{
	HList<JGType> _typeList;
	HList<uint8>  _byteDatas;
public:
	virtual ~PPayload() = default;

	int32 GetNumData() const
	{
		return (int32)_typeList.size();
	}

	template<class T>
	bool GetData(int32 inIndex, T* OutValue) const
	{
		if (inIndex >= GetNumData())
		{
			return false;
		}

		bool bFind = false;
		for (const JGType& type : _typeList)
		{
			if (type == JGTYPE(T))
			{
				bFind = true;
			}
		}

		if (bFind == false)
		{
			return false;
		}

		uint64 DataOffset = GetDataOffset(inIndex);
	
		memcpy_s(OutValue, sizeof(T), &(_byteDatas[DataOffset]), sizeof(T));

		return true;
	}


private:
	uint64 GetDataOffset(int32 inIndex) const
	{
		int32 NumData = GetNumData();
		if (inIndex >= NumData)
		{
			return;
		}

		uint64 Offset = 0;

		for (int32 i = 0; i < inIndex; ++i)
		{
			if (inIndex == inIndex)
			{
				return Offset;
			}

			Offset += _typeList[i].GetSize();
		}

		return 0;
	}
};



