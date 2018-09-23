#include"JG2DFilter.h"
using namespace std;
JG2DFilter::JG2DFilter()
{
	m_Filter = make_unique<b2Filter>();
}

JG2DFilter::JG2DFilter(JG2DFilter& copy)
{
	m_Filter = make_unique<b2Filter>();
	m_Filter->categoryBits = copy.Get().categoryBits;
	m_Filter->groupIndex = copy.Get().groupIndex;
	m_Filter->maskBits = copy.Get().maskBits;
}

JG2DFilter::~JG2DFilter()
{
}

b2Filter* JG2DFilter::GetData()
{
	return m_Filter.get();
}

b2Filter& JG2DFilter::Get()
{
	return *m_Filter;
}

void JG2DFilter::Set(const b2Filter & filter)
{
	*m_Filter = filter;
}

JG2DFilter& JG2DFilter::operator=(JG2DFilter& filter)
{
	m_Filter->categoryBits = filter.Get().categoryBits;
	m_Filter->groupIndex = filter.Get().groupIndex;
	m_Filter->maskBits = filter.Get().maskBits;

	return *this;
}
