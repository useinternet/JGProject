// JGLibTest.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

#include<crtdbg.h>
#include"StringUtil.h"
#include"coordinateMath.h"
using namespace std;
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	string str = "  s  ";
	StringUtil::DelBlank(str);
    return 0;
}

