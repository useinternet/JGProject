#include "pch.h"
#include "JGUIForm.h"
#include "GUI/JGUIComponent/JGUIShape.h"
#include "GUI/JGUIComponent/JGUIPanel.h"
#include "GUI/JGUIComponent/JGUIRectTransform.h"
#include "GUI/JGUIComponent/JGUIButton.h"
#include "GUI/JGUIComponent/JGUICollider.h"
using namespace std;


void JGUIForm::Start()
{
	rect1 = CreateJGUIComponent<JGUIPanel>("Rect");
	rect1->SetColor({ 1.0f,0.0f,0.0f, 1.0f });
	rect1->GetTransform()->SetSize({ 100,100 });
	rect1->RegisterBoxCollider();
	rect2 = CreateJGUIComponent<JGUIButton>("JGUIButton");
	rect2->GetTransform()->SetPosition(0.0f, 0.0f);
	rect2->SetBtImage("Maximum.png");
	rect2->GetTransform()->OffsetPosition(300, 400);
	rect2->GetCollider()->DebugOn();
}

void JGUIForm::Tick(const JGUITickEvent& e)
{

}

void JGUIForm::KeyDown(const JGUIKeyDownEvent& e)
{
	if (e.Code == KeyCode::Right)
	{
		rect2->GetTransform()->OffsetPosition({ 1.0f,0.0f });
	}
	if (e.Code == KeyCode::Left)
	{
		rect2->GetTransform()->OffsetPosition({ -1.0f,0.0f });
	}
	if (e.Code == KeyCode::Down)
	{
		rect2->GetTransform()->OffsetPosition({0.0f, 1.0f });
	}
	if (e.Code == KeyCode::Up)
	{
		rect2->GetTransform()->OffsetPosition({ 0.0f,-1.0f });
	}


}
void JGUIForm::KeyUp(const JGUIKeyUpEvent& e)
{
	ENGINE_LOG_INFO(e.Code.ToString());
	if (e.Code == KeyCode::K)
	{
		rect2->GetTransform()->OffsetPivot({ 1.0f,0.0f });
	}
	if (e.Code == KeyCode::J)
	{
		rect2->GetTransform()->OffsetPivot({ -1.0f,0.0f });
	}
	if (e.Code == KeyCode::I)
	{
		rect2->GetTransform()->OffsetPivot({ 0.0f,1.0f });
	}
	if (e.Code == KeyCode::M)
	{
		rect2->GetTransform()->OffsetPivot({ 0.0f,-1.0f });
	}
}
