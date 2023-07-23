#include "PCH/PCH.h"




#include <iostream>
#include "Core.h"
#include "TestClass/TestClassA.h"


using namespace std;

void test()
{

}
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


