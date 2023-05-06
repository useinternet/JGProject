#include "PCH/PCH.h"
#include "Guid.h"


// Window Platform
HGuid::HGuid()
{
	_data1 = 0;
	_data2 = 0;
	_data3 = 0;
	for (int32 i = 0; i < 8; ++i)
	{
		_data4[i] = 0;
	}
}

bool HGuid::IsValid() const
{
	if (_data1 == 0)
	{
		return false;
	}

	return true;
}

bool HGuid::operator==(const HGuid& inGuid) const
{
	if (_data1 != inGuid._data1 ||
		_data2 != inGuid._data2 ||
		_data3 != inGuid._data3)
	{
		return false;
	}

	for (int32 i = 0; i < 8; ++i)
	{
		if (_data4[i] != inGuid._data4[i])
		{
			return false;
		}
	}

	return true;
}

bool HGuid::operator!=(const HGuid& inGuid) const
{
	if (_data1 != inGuid._data1 ||
		_data2 != inGuid._data2 ||
		_data3 != inGuid._data3)
	{
		return true;
	}

	for (int32 i = 0; i < 8; ++i)
	{
		if (_data4[i] != inGuid._data4[i])
		{
			return true;
		}
	}

	return false;
}


const PString& HGuid::ToString() const
{
	if (_dataStr.Empty())
	{
		PString dataStr1 = PString::FromUint64(_data1);
		PString dataStr2 = PString::FromUint32((uint32)_data2);
		PString dataStr3 = PString::FromUint32((uint32)_data3);

		PString dataStr4;
		for (int32 i = 0; i < 8; ++i)
		{
			dataStr4 += "-";
			dataStr4 += PString::FromUint32(_data4[i]);
		}

		_dataStr = dataStr1 + "-" + dataStr2 + "-" + dataStr3 + dataStr4;
	}

	return _dataStr;
}

uint64 HGuid::GetHashCode() const
{
	return HPlatform::GuidHash(*this);
}

HGuid HGuid::New()
{
	return HPlatform::NewGuid();
}

HGuid HGuid::FromString(const PString& inGuidStr)
{
	HList<PString> tokens = inGuidStr.Split('-');
	if (tokens.size() < 11)
	{
		return HGuid();
	}

	HGuid guid;
	guid._data1 = tokens[0].ToUint64();
	guid._data2 = tokens[1].ToUint32();
	guid._data3 = tokens[2].ToUint32();
	guid._data4[0] = tokens[3].ToUint32();
	guid._data4[1] = tokens[4].ToUint32();
	guid._data4[2] = tokens[5].ToUint32();
	guid._data4[3] = tokens[6].ToUint32();
	guid._data4[4] = tokens[7].ToUint32();
	guid._data4[5] = tokens[8].ToUint32();
	guid._data4[6] = tokens[9].ToUint32();
	guid._data4[7] = tokens[10].ToUint32();

	return guid;
}
