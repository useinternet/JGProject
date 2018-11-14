#pragma once
#include<Windows.h>
// 익스포트 정의
#ifdef JGMAINCORE_EXPORTS
#define MAINCORE_EXPORT __declspec(dllexport)
#else
#define MAINCORE_EXPORT __declspec(dllimport)
#endif

#ifdef _DEBUG
#pragma comment(lib,"JGCommon/JGCommon_d.lib")
#else
#pragma comment(lib,"JGCommon/JGCommon.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"JGRenderCore_d.lib")
#else
#pragma comment(lib,"JGRenderCore.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"JGLogicCore_d.lib")
#else
#pragma comment(lib,"JGLogicCore.lib")
#endif
namespace JGRC { class JGRenderCore; }
namespace JGLC { class JGLogicCore; }
class MAINCORE_EXPORT JGMainCore
{
private:
	bool m_bEdit  = false;
	bool m_Paused = false;
	JGRC::JGRenderCore* m_JGRC;
	JGLC::JGLogicCore* m_JGLC;
	// 실험요
	HWND hWnd;
public:
	JGMainCore();
	~JGMainCore();

	bool InitCore(HWND hWnd);
	void Run();
	void Destroy();

	// 에디터 용
	void Run_Edt();
	void ReceiveCommand(const unsigned int Command, void* Data, unsigned int size);
};