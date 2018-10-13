#include"DamageInformationBase.h"

DamageInformationBase::DamageInformationBase()
{

}

DamageInformationBase::~DamageInformationBase()
{

}



EDamageType DamageInformationBase::GetType()
{
	return m_Type;
}
void DamageInformationBase::SetType(EDamageType type)
{
	m_Type = type;
}