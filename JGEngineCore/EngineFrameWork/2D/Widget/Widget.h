#pragma once
#include"../../Components/Motivated2DComponent.h"
class Object;

enum class WidgetState
{
	Active,
	Behind

};
class ENGINE_EXPORT Widget : public Motivated2DComponent
{
private:
	Object* m_pObject = nullptr;
	WidgetState m_CurrentState;
public:
	Widget();
	virtual ~Widget();
	void BindingObject(Object* object);
	Object* GetBindingObject();
	void BehindWidget();
	void ViewWidget();
};