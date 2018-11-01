#pragma once
#include<Windows.h>
// 익스포트 정의
#ifdef JGMAINCORE_EXPORTS
#define MAINCORE_EXPORT __declspec(dllexport)
#else
#define MAINCORE_EXPORT __declspec(dllimport)
#endif

#ifdef _DEBUG
#pragma comment(lib,"JGRenderCore_d.lib")
#else
#pragma comment(lib,"JGRenderCore.lib")
#endif

namespace JGRC { class JGRenderCore; }

class MAINCORE_EXPORT JGMainCore
{
private:
	bool m_Paused = false;
	JGRC::JGRenderCore* m_JGRC;
public:
	JGMainCore();
	~JGMainCore();

	bool InitCore(HWND hWnd);
	void Run();
	void Destroy();
};