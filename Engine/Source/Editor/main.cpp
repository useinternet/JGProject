#include "pch.h"
#include <crtdbg.h>
#include "Resources/resource.h"
#include "JGCore.h"
#include "JGEditor.h"


using namespace JG;


int main(int argc, char** argv)
{
	ApplicaionProps props;
	props.WindowPlatform = EWindowPlatform::Windows;
	props.WindowProps.TitleName = L"JGEditor";
	props.WindowProps.Width = 1920;
	props.WindowProps.Height = 1080;
	props.WindowProps.IconID = IDI_ICON1;
	Application* app = new JGEditor();

	if (app->Create(props))
	{
		app->Run();
	}

	app->Destroy();

	delete app;
	app = nullptr;
	return 0;
}
