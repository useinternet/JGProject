#pragma once
#include"../../EngineStatics/Engine.h"
enum class ENGINE_EXPORT EDamageType
{
	None,
	Single,
	Splash
};
class ENGINE_EXPORT  DamageInformationBase
{
private:
	EDamageType m_Type = EDamageType::None;
public:
	DamageInformationBase();
	virtual ~DamageInformationBase();

	EDamageType GetType();
	void SetType(EDamageType type);
};
