#pragma once
#pragma warning (disable : 4996 4005 4251)
#include<JGCommon/JGCommon.h>
#ifdef _DEBUG
#pragma comment(lib,"JGCommon/JGCommon_d.lib")
#else
#pragma comment(lib,"JGCommon/JGCommon.lib")
#endif
#include"dllExportDefine.h"

/* 윈도우즈 헤더파일  */
//Windows

#include<Windows.h>
#include<wrl.h>
//C++
#include<string>
#include<fstream>
#include<iostream>
//STL
// - 컨테이너
#include<vector>
#include<queue>
#include<stack>
#include<unordered_map>
#include<unordered_set>
#include<map>
#include<set>
#include<list>
#include<forward_list>
#include<array>
#include<algorithm>
// - 쓰레드
#include<thread>
#include<atomic>
#include<condition_variable>
#include<mutex>
// - 함수 및 포인터 관련
#include<memory>
#include<functional>

// - 시간
#include<chrono>

//- C언어
#include<stdlib.h>



// 임시
namespace Game
{
	static JGPath::Path path("../../../../../../../Contents");
	//static JGPath::Path path("../../Contents");
}



