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
Module 인자

Module: SharedLib, StaticLib, ConsoleApp
ModuleDependency: { Core, ... }
Defines:



*/

bool PBuildTool::Run()
{
	// module.json 파일 수집 및 모듈 이름과 source 파일 위치 매칭

	// lua 코드 작성

	// premake 생성
}