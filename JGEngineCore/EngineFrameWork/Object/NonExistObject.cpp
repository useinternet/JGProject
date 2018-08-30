#include"NonExistObject.h"


NonExistObject::NonExistObject()
{
	RegisterObjectID(typeid(this));
}
NonExistObject::~NonExistObject()
{

}