// JGLibTest.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include<crtdbg.h>
#include<stdio.h>
using namespace std;
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	int n;
	printf("=========================");
	printf(" n        n의 제곱 /\n");
	printf("=========================");

	n = 1;
	while (n <= 10)
	{
		printf("%5d %5d\n", n, n*n);
		n++;
	}
    return 0;
}

