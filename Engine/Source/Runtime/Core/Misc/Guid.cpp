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


PString HGuid::ToString() const
{
	PString dataStr1 = PString::FromUint64(_data1);
	PString dataStr2 = PString::FromUint32((uint32)_data2);
	PString dataStr3 = PString::FromUint32((uint32)_data3);

	PString dataStr4;
	for (int32 i = 0; i < 8; ++i)
	{
		dataStr4 += PString::FromUint32(_data4[i]);
	}

	if (_dataStr.Empty())
	{
		_dataStr = dataStr1 + "-" + dataStr2 + "-" + dataStr3 + "-" + dataStr4;
	}

	return _dataStr;
}

HGuid HGuid::New()
{
	return HPlatform::NewGuid();
}
