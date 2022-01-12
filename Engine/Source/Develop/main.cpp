#include "pch.h"
#include <crtdbg.h>
#include "JGCore.h"
#include "Develop.h"


using namespace JG;


int main(int argc, char** argv)
{
	ApplicaionProps props;
	props.WindowPlatform = EWindowPlatform::Windows;
	props.WindowProps.TitleName = L"Raytracing";
	props.WindowProps.Width  = 1920;
	props.WindowProps.Height = 1080;
	Application* app = new Develop();
	if (app->Create(props))
	{
		app->Run();
	}

	app->Destroy();

	delete app;
	app = nullptr;
	return 0;
}
