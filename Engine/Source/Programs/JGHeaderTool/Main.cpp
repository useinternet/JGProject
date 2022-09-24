

//
#include "Core.h"

#include "Test/TestObject.h"
#include <iostream>

int main()
{
	GCoreSystem::Create(ECoreSystemFlags::No_CodeGen);




	GCoreSystem::Destroy();
	return 0;
}