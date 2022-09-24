#include "Module.h"

IModuleInterface* GModuleGlobalSystem::FindModule(const JGType& type)
{
	return nullptr;
}

bool GModuleGlobalSystem::ConnectModule(const JGType& type)
{
	// dll
	return false;
}

bool GModuleGlobalSystem::DisconnectModule(const JGType& type)
{
	return false;
}

bool GModuleGlobalSystem::ReconnectModule(const JGType& type)
{
	return false;
}