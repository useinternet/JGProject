#include"JGRCObject.h"
#include"DirectX/DirectX.h"
unsigned long long JGRC::JGRCObject::Count = 0;

JGRC::DirectX* JGRC::JGRCObject::m_Dx = nullptr;

JGRC::JGRCObject::JGRCObject()
{
	m_Dx = DirectX::GetInstance();
	Count++; 
}
JGRC::DirectX* JGRC::JGRCObject::GetDx()
{
	return m_Dx;
}