#pragma once
#include"../../EngineStatics/Engine.h"


class Object;
class ENGINE_EXPORT WorldCompare
{
public:
	bool operator()(std::shared_ptr<Object> object1, std::shared_ptr<Object> object2);
};