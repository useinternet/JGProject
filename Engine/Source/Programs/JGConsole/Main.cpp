



#include <iostream>
#include "Core.h"

class Test
{
public:
	HList<float> ss;
	HHashSet<int> ss2;

};


using namespace std;
int main()
{
	//ACoreSystem::Create();

	Test t;
	Test t2;
	t = t2;

	//t = t2;
	cout << "test" << endl;


	//ACoreSystem::Destroy();
	return 0;
}