#pragma once
#include"../../../EngineStatics/Engine.h"
#include"../JGCommon/b2ToJGCommon.h"


class ENGINE_EXPORT JG2DFilter
{
	std::unique_ptr<b2Filter> m_Filter;


public:
	JG2DFilter();
	JG2DFilter(JG2DFilter& copy);
	~JG2DFilter();

	b2Filter* GetData();
	b2Filter& Get();
	void Set(const b2Filter& filter);

	JG2DFilter& operator=(JG2DFilter& filter);
};