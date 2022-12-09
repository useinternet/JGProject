#include "PCH/PCH.h"
#include "Core.h"

using namespace std;

int main()
{
	HCoreSystemArguments args;
	args.LaunchModule = "JGEditor";

	GCoreSystem::Create(args);

	while (GCoreSystem::Update()) {}

	GCoreSystem::Destroy();
	return 0;
}


