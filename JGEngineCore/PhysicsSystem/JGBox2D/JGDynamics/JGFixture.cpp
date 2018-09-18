#include"JGFixture.h"
#include"JG2DBody.h"
JGFixture::JGFixture()
{
}

JGFixture::~JGFixture()
{
}

b2Fixture* JGFixture::Get()
{
	return m_Fixture;
}

void JGFixture::CreateFixture(b2Fixture * fixture)
{
	m_Fixture = fixture;
}

void JGFixture::Set(b2Fixture* fixture)
{
	m_Fixture = fixture;
}
