#include "Core.h"
#include "BuildTool.h"

// PString

int32 main(int32 argc, char* argv[])
{
	GCoreSystem::Create(ECoreSystemFlags::No_CodeGen);

	PBuildTool BuildTool;
	if (BuildTool.Run() == false)
	{
		return -1;
	}

	GCoreSystem::Destroy();

	return 0;
}