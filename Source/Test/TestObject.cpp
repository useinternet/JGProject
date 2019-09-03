#include "TestObject.h"




void TestObject::Begin()
{
	GameObject::Begin();
	
	JG_LOG_TRACE("Begin");
}

void TestObject::Tick()
{
	GameObject::Tick();
	JG_LOG_TRACE("Tick");
}

void TestObject::End()
{
	GameObject::End();
	JG_LOG_TRACE("End");
}
