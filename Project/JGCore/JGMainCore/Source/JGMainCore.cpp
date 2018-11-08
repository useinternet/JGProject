#include"JGMainCore.h"
#include"JGRenderCore.h"
#include"JGLogicCore.h"
#include<JGCommon/JGCommon.h>
using namespace JGRC;
using namespace JGLC;
JGMainCore::JGMainCore() {}
JGMainCore::~JGMainCore() {}

bool JGMainCore::InitCore(HWND hWnd)
{
	JGLOGINIT("EngineCoreLog.log");
	m_JGRC = new JGRenderCore;
	m_JGRC->Init(hWnd);
	m_JGLC = new JGLogicCore;
	m_JGLC->TestInit(m_JGRC);
	return true;
}
void JGMainCore::Run()
{
	MSG msg;
	while (!m_Paused)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			m_Paused = true;
		}
		m_JGLC->TestTick();
		m_JGRC->Draw();
	}
}
void JGMainCore::Run_Edt()
{
	m_JGLC->TestTick();
	m_JGRC->Draw();
}
void JGMainCore::Destroy()
{
	delete m_JGRC;
	m_JGRC = nullptr;
	delete m_JGLC;
	m_JGLC = nullptr;
}
void JGMainCore::TestPage()
{

}