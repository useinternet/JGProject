

//
#include "Core.h"
#include <iostream>


using namespace std;
int main()
{
	ACoreSystem::Create();

	PWeakPtr<int32> testw;
	{
		PSharedPtr<int32> test = AMemoryGlobalSystem::GetInstance().Allocate<int32>();

		*test = 3;

		testw = test;

		if (testw.IsValid())
		{
			cout << "valid" << endl;
		}
		PSharedPtr<int32> test1 = test;

		
	}

	if (testw.IsValid())
	{
		cout << "valid" << endl;
	}

	AMemoryGlobalSystem::GetInstance().Flush();
	







	ACoreSystem::Update();
	

	ACoreSystem::Destroy();
	return 0;
}