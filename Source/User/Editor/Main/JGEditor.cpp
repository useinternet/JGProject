#include "pch.h"
#include "JGEditor.h"
#include "Form/Edit/Viewport.h"
#include "Form/Edit/Detail.h"
#include "Form/Develop/Develop.h"
#include "Form/Develop/UnitTest.h"
using namespace std;




void JGEditor::Awake()
{
	GetTransform()->SetSize(800, 600);


}

void JGEditor::Start()
{

	//auto clientRect = GetClientRect();
	//float offsetX = clientRect.left;

	//auto develop = CreateWindow<Detail>(TT("DevelopTest"), (JGUI::WindowFlags)(JGUI::WindowFlag_ChildWindow | JGUI::WindowFlag_NoMove));
	//develop->GetTransform()->SetSize(383, 648);
	//develop->GetTransform()->SetLocalLocation(offsetX, clientRect.top);

	//offsetX += 383;

	//JGEditorHub::viewport = CreateWindow<Viewport>(TT("Viewport"), (JGUI::WindowFlags)(JGUI::WindowFlag_ChildWindow | JGUI::WindowFlag_NoMove));
	//JGEditorHub::viewport->GetTransform()->SetSize(1152, 648);
	//JGEditorHub::viewport->GetTransform()->SetLocalLocation(offsetX, clientRect.top); offsetX += 1152;

	//JGEditorHub::detail = CreateWindow<Detail>(TT("Detail"), (JGUI::WindowFlags)(JGUI::WindowFlag_ChildWindow | JGUI::WindowFlag_NoMove));
	//JGEditorHub::detail->GetTransform()->SetSize(383, 1020);
	//JGEditorHub::detail->GetTransform()->SetLocalLocation(offsetX, clientRect.top); offsetX += 383;

}

void JGEditor::Tick()
{



}

void JGEditor::Destroy()
{



}

void JGEditor::OnKeyUp(KeyCode code)
{

	if (code == KeyCode::Tab)
	{
		if (JGEditorHub::unitTest == nullptr)
		{
			auto clientRect = GetClientRect();
			JGEditorHub::unitTest = CreateWindow<UnitTest>(TT("UnitTest"), JGUI::WindowFlag_NoMove);
			JGEditorHub::unitTest->GetTransform()->SetSize(clientRect.Width(), clientRect.Height());
			JGEditorHub::unitTest->GetTransform()->SetLocalLocation(clientRect.left, clientRect.top);
			JGEditorHub::unitTest->GetTransform()->SetAnchor(JGUI::AnchorPreset::Full_Stretch);
		}
		else
		{
			DestroyWindow(JGEditorHub::unitTest);
			JGEditorHub::unitTest = nullptr;
		}

	}


}
