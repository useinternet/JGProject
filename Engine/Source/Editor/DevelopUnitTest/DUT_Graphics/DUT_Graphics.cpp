#include "PCH/PCH.h"
#include "DUT_Graphics.h"
#include "WidgetComponents/WText.h"

JG_REGISTER_DEVELOP_UNIT(JGDUT_Graphics, DUT_GRAPHICS_C_API)


void JGDUT_Graphics::Startup()
{
	GCoreSystem::Create();

	JG_LOG(DUT_Graphics, ELogLevel::Info, "Start up");
}

void JGDUT_Graphics::Update()
{
	GCoreSystem::Update();
}

void JGDUT_Graphics::Shutdown()
{
	GCoreSystem::Destroy();

	JG_LOG(DUT_Graphics, ELogLevel::Info, "Shutdown");
}

PSharedPtr<WWidgetComponent> JGDUT_Graphics::CreateContentWidgetComponent()
{
	WText::HArguments args;
	args.Text = "JGDUT_Gragphis";

	PSharedPtr<WText> TestText = NewWidgetComponent<WText>(args);
	return TestText;
}
