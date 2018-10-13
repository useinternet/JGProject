#pragma once

#include"DamageInformationBase.h"

class ENGINE_EXPORT SingleDamage : public DamageInformationBase
{
public:
	class Object* DamagedObject = nullptr;
	float   Damage = 0.0f;
public:
	SingleDamage();
	virtual ~SingleDamage();
};