#include"Widget.h"


Widget::Widget()
{
	RegisterComponentID(typeid(this));
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


