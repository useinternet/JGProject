#include "PCH/PCH.h"
#include "JGGraphics.h"

JG_MODULE_IMPL(HJGGraphicsModule, GRAPHICS_C_API)

JGType HJGGraphicsModule::GetModuleType() const
{
	return JGTYPE(HJGGraphicsModule);
}

void HJGGraphicsModule::StartupModule()
{

}

void HJGGraphicsModule::ShutdownModule()
{

}

PSharedPtr<PJGGraphicsAPI> HJGGraphicsModule::GetGraphicsAPI() const
{
	return _graphicsAPI;
}
