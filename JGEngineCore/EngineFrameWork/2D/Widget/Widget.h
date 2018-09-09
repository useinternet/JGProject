#pragma once
#include"../../Components/Motivated2DComponent.h"
class Object;


class ENGINE_EXPORT Widget : public Motivated2DComponent
{
private:
	Object* m_pObject = nullptr;
public:
	Widget();
	virtual ~Widget();
	void BindingObject(Object* object);
	Object* GetBindingObject();
};