#include "BuildTool.h"

/*
		project "JGBuildTool"
			includedirs{
				"Source/Programs/JGBuildTool/",
				"Source/Runtime/Core/",
				"Source/ThirdParty/",
			}

			links{
				"Core"
			}
			SetCPPProjectConfig("ConsoleApp", "Source/Programs/JGBuildTool/", "_JGBUILDTOOL")
			filter "configurations:DevelopEngine"
				DebugConfig()
			filter "configurations:DevelopGame"
				ConfirmConfig()
			filter "configurations:ConfirmGame"
				ConfirmConfig()
			filter "configurations:ReleaseGame"
				ReleaseConfig()
*/

/*
Module ����

Module: SharedLib, StaticLib, ConsoleApp
ModuleDependency: { Core, ... }
Defines:



*/

bool PBuildTool::Run()
{
	// module.json ���� ���� �� ��� �̸��� source ���� ��ġ ��Ī

	// lua �ڵ� �ۼ�

	// premake ����
}