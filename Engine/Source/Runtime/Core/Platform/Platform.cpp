#include "PCH/PCH.h"
#include "Platform.h"
#include "JWindow.h"
#include "Misc/Guid.h"

#ifdef _PLATFORM_WINDOWS
#include "Windows/WindowsJWindow.h"
#endif

HJInstance HPlatform::LoadDll(const PString& dllPath)
{
#ifdef _PLATFORM_WINDOWS
	return ::LoadLibraryA(dllPath.GetCStr());
#else
	return 0;
#endif
}


void HPlatform::UnLoadDll(HJInstance instanceID)
{
	if (instanceID == nullptr)
	{
#ifdef _PLATFORM_WINDOWS
		::FreeLibrary(instanceID);
#else
#endif
	}
}

PSharedPtr<PJWindow> HPlatform::CreateJWindow(const HJWindowArguments& args)
{
#ifdef _PLATFORM_WINDOWS
	PSharedPtr<PJWindow> window = ::Allocate<PWindowsJWindow>();
	HJWindowCreater windowCreater(window.GetRawPointer(), args);
	if (windowCreater.IsSuccess())
	{
		return window;
	}

	return nullptr;
#else 
	return nullptr;
#endif
}

HGuid HPlatform::NewGuid()
{
	HGuid Guid;
#ifdef _PLATFORM_WINDOWS
	UUID newId;
	UuidCreate(&newId);

	Guid._data1 = newId.Data1;
	Guid._data2 = newId.Data2;
	Guid._data3 = newId.Data3;

	for (int32 i = 0; i < 8; ++i)
	{
		Guid._data4[i] = newId.Data4[i];
	}

#endif

	return Guid;
}

uint64 HPlatform::GuidHash(const HGuid& inGuid)
{
	if (inGuid.IsValid())
	{
		return 0;
	}

	uint64 hash = 0;

#ifdef _PLATFORM_WINDOWS
	UUID uuid;

	uuid.Data1 = inGuid._data1;
	uuid.Data2 = inGuid._data2;
	uuid.Data3 = inGuid._data3;
	for (int32 i = 0; i < 8; ++i)
	{
		uuid.Data4[i] = inGuid._data4[i];
	}

	hash = ::UuidHash(&uuid, nullptr);

#endif
	return hash;
}
