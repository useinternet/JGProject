#include "pch.h"
#include "JGUICanvas.h"


using namespace std;

void JGUICanvas::Awake()
{

}

void JGUICanvas::Destroy()
{

}

void JGUICanvas::Tick(float tick)
{
	if (m_CanvasFlags & JGUI_CanvasFlag_TopMost)
	{
		m_Layer = JGUI_WindowPriority_TopMost + 1;
	}
}
