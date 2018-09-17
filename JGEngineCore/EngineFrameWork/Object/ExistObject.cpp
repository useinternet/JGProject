#include"ExistObject.h"

using namespace std;
ExistObject::ExistObject()
{
	RegisterObjectID(typeid(this));
}
ExistObject::~ExistObject()
{

}






