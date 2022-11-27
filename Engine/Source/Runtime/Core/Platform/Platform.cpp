#include "PCH/PCH.h"
#include "Platform.h"
#include "JWindow.h"

HJInstance HPlatform::LoadDll(const PString& dllPath)
{
	return ::LoadLibraryA(dllPath.GetCStr());
}


void HPlatform::UnLoadDll(HJInstance instanceID)
{
	if (instanceID == nullptr)
	{
		::FreeLibrary(instanceID);
	}
}

PSharedPtr<PJWindow> HPlatform::CreateJWindow(const HJWindowArguments& args)
{
	return nullptr;
}