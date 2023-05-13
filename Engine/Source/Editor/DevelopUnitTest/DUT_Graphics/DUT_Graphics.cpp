#include "PCH/PCH.h"
#include "DUT_Graphics.h"

JG_REGISTER_DEVELOP_UNIT(JGDUT_Graphics, DUT_GRAPHICS_C_API)


void JGDUT_Graphics::Startup()
{
	GCoreSystem::Create();
}

void JGDUT_Graphics::Update()
{
	GCoreSystem::Update();
}

void JGDUT_Graphics::Shutdown()
{
	GCoreSystem::Destroy();
}

