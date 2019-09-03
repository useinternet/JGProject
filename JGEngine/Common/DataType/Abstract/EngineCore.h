#pragma once
#include <queue>
#include <functional>
#include "NoneCopy.h"
class Event;
struct GlobalLinkStream;
class GlobalLinkData;
class EngineCore : public NoneCopy
{
public:
	EngineCore(const GlobalLinkStream& stream);
	virtual ~EngineCore();
public:
	virtual void Load() = 0;
	virtual void Update() = 0;
	virtual void OnEvent(Event& e) = 0;
};
