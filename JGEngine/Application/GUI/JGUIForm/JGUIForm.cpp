#include "pch.h"
#include "JGUIForm.h"
#include "GUI/JGUIComponent/JGUIShape.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUIButton.h"
#include "GUI/JGUIComponent/JGUICollider.h"
#include "GUI/JGUIComponent/JGUITitleBar.h"
using namespace std;


void JGUIForm::Start()
{
	title = CreateJGUIComponent<JGUITitleBar>("titleBar");

}

void JGUIForm::Tick(const JGUITickEvent& e)
{

}

void JGUIForm::KeyDown(const JGUIKeyDownEvent& e)
{
}

void JGUIForm::KeyUp(const JGUIKeyUpEvent& e)
{
}
