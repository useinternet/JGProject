#include "PCH/PCH.h"
#include "Guid.h"
#include "Platform/Platform.h"

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

HGuid HGuid::New()
{
	return HPlatform::NewGuid();
}
