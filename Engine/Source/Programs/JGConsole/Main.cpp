



#include <iostream>
#include "Core.h"


using namespace std;
int main()
{
	GCoreSystem::Create();

	while (true)
	{
		PDateTime now = PDateTime::Now();

		PString test1 = now.ToString();

		PDateTime test2;
		test2.Parse(test1);

		PString test3 = test2.ToString();

		PDateTime end = PDateTime::Now();
		PTimespan delta = end - now;

		PString test4 = delta.ToString();


		GCoreSystem::Update();
	}

	GCoreSystem::Destroy();
	return 0;
}