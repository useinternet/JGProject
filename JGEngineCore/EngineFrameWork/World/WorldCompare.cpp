#include "WorldCompare.h"
#include"../Object/Object.h"
using namespace std;



bool WorldCompare::operator()(shared_ptr<Object> object1, shared_ptr<Object> object2)
{
	if (object1->GetZOrder() < object2->GetZOrder())
	{
		return true;
	}
	return false;
}
