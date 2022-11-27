#include "PCH/PCH.h"
#include "Core.h"

using namespace std;

int main()
{
	GCoreSystem::Create();
	
	while (true)
	{
		GCoreSystem::Update();
	}

	GCoreSystem::Destroy();
	return 0;
}


