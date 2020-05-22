#include "pch.h"
#include "CommonCore.h"

EngineCore::EngineCore(const GlobalLinkStream& stream)
{
	GlobalLinkData::Init(stream);
}

EngineCore::EngineCore()
{
}

EngineCore::~EngineCore()
{

}
