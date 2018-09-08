#include"Widget.h"


Widget::Widget()
{
	m_CurrentState = WidgetState::Active;
}
Widget::~Widget()
{

}



void Widget::BindingObject(Object* object)
{
	m_pObject = object;
}
Object* Widget::GetBindingObject()
{
	return m_pObject;
}


void Widget::BehindWidget()
{
	m_CurrentState = WidgetState::Active;
}
void Widget::ViewWidget()
{
	m_CurrentState = WidgetState::Behind;
}