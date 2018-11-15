// JGLibTest.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include<crtdbg.h>
#include<stdio.h>
//#include<JGCommon/JGCommon.h>
//using namespace std;
//#ifdef _DEBUG
//#pragma comment(lib,"JGCommon/JGCommon_d")
//#else
//#pragma comment(lib,"JGCommon/JGCommon")
//#endif
#include<iostream>
#include<vector>
using namespace std;

void PrintVector(const vector<int>& v)
{
	for (auto& e : v)
	{
		cout << e << endl;
	}
}
double getAverage(const vector<int> & v)
{
	int sum = 0;

	for (auto&e : v)
	{
		sum += e;
	}
	return sum / v.size();
}
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	vector<int> a;
	for (int i = 0; i < 100; ++i)
	{
		a.push_back(i);
	}
	PrintVector(a);




    return 0;
}

