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
	EngineCore();
	virtual ~EngineCore();
public:
	virtual void Init() {}
	virtual void Load() {}
	virtual void Update() {}
	virtual void Destroy() {}
	virtual void OnEvent(Event& e) {}
};

template<typename EnginCoreType>
inline EngineCore* CreateEngineCore(const GlobalLinkStream& stream)
{
	EngineCore* obj = new EnginCoreType(stream);
	return obj;
}


//
#define REGISTER_GRAPHICSENGINE(ClassName) \
extern "C" __declspec(dllexport) \
inline  EngineCore* CreateGraphicsEngine(const GlobalLinkStream& stream)\
{ \
	return CreateEngineCore<ClassName>(stream);  \
} \









