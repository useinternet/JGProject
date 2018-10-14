#include"UserWidget.h"

UserWidget::UserWidget()
{
	RegisterObjectID(typeid(this));
	BehindObject();
	CullingIgnore();
}

UserWidget::~UserWidget()
{
}
