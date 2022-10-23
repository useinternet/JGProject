local GEN_PROJECT_FILE_PATH = "Temp/ProjectFiles/"
local BIN_PATH        = "Bin/%{cfg.buildcfg}/"
local OBJECT_PATH     = "Temp/Obj/%{cfg.buildcfg}/"
--local PCH_HEADER      = "PCH.h"
--local PCH_HEADER_PATH = "Source/PCH/PCH.h"
--local PCH_CPP_PATH    = "Source/PCH/PCH.cpp"

function DebugConfig(defined)
    symbols       "On"
    optimize      "Off"
    defines       {"_DEBUG", "_PROFILE"}
    cppdialect    "C++20"
    staticruntime "off"
    runtime       "Debug"
end

function ConfirmConfig(defined)
    optimize        "Full" 
    defines         {"_RELEASE", "NDEBUG", "_PROFILE"}
    cppdialect      "C++20"
    staticruntime   "off"
    runtime         "Release"
end

function ReleaseConfig(defined)
    optimize        "Full" 
    defines         {"_RELEASE", "NDEBUG"}
    cppdialect      "C++20"
    staticruntime   "off"
    runtime         "Release"
end

workspace "JGEngine"
    architecture "x64"

    configurations
    { 
        "DevelopEngine",  -- Engine : Debug,  Game : Debug,  Profile On
        "DevelopGame",    -- Engine : Release, Game : Debug, Profile On 
        "ConfirmGame",    -- Engine : Release, Game : Release, Profile On
        "ReleaseGame",    -- Engine : Release, Game : Release, Profile Off
    }

    -- "StaticLib"  --
    -- "SharedLib"  --
    -- "ConsoleApp" --
    function SetCPPProjectConfig(kind_name, path, defined)
        location  (GEN_PROJECT_FILE_PATH)
        kind (kind_name)
        language "C++"
        debugdir  (BIN_PATH)
        targetdir (BIN_PATH)
        objdir(OBJECT_PATH)
        pchheader (PCH_HEADER)
        pchsource (PCH_CPP_PATH)
        if defined ~= nil then
            defines {defined}
        end
        -- file
        files {
            path .. "**.h",
            path .. "**.cpp",
            path .. "**.c",
            --PCH_HEADER_PATH,
            --PCH_CPP_PATH,
        }
    end

    group "Engine"
		group "Engine/Programs"
			project "JGBuildTool"
				includedirs{ "Source/Programs/JGBuildTool/", "Source/ThirdParty", "Source/Runtime/Core/", }
				links{ "Core", }
				SetCPPProjectConfig("ConsoleApp", "Source/Programs/JGBuildTool/", {"_JGBUILDTOOL", })
				filter "configurations:DevelopEngine"
					DebugConfig()
					defines{"_DEVELOPENGINE"}
				filter "configurations:DevelopGame"
					ConfirmConfig()
					defines{"_DEVELOPGAME"}
				filter "configurations:ConfirmGame"
					ConfirmConfig()
					defines{"_CONFIRMGAME"}
				filter "configurations:ReleaseGame"
					ReleaseConfig()
					defines{"_RELEASEGAME"}


			project "JGConsole"
				includedirs{ "Source/Programs/JGConsole/", "Source/ThirdParty", "Source/Runtime/Core/", }
				links{ "Core", }
				SetCPPProjectConfig("ConsoleApp", "Source/Programs/JGConsole/", {"_JGCONSOLE", })
				filter "configurations:DevelopEngine"
					DebugConfig()
					defines{"_DEVELOPENGINE"}
				filter "configurations:DevelopGame"
					ConfirmConfig()
					defines{"_DEVELOPGAME"}
				filter "configurations:ConfirmGame"
					ConfirmConfig()
					defines{"_CONFIRMGAME"}
				filter "configurations:ReleaseGame"
					ReleaseConfig()
					defines{"_RELEASEGAME"}


			project "JGHeaderTool"
				includedirs{ "Source/Programs/JGHeaderTool/", "Source/ThirdParty", "Source/Runtime/Core/", }
				links{ "Core", }
				SetCPPProjectConfig("ConsoleApp", "Source/Programs/JGHeaderTool/", {"_JGHEADERTOOL", })
				filter "configurations:DevelopEngine"
					DebugConfig()
					defines{"_DEVELOPENGINE"}
				filter "configurations:DevelopGame"
					ConfirmConfig()
					defines{"_DEVELOPGAME"}
				filter "configurations:ConfirmGame"
					ConfirmConfig()
					defines{"_CONFIRMGAME"}
				filter "configurations:ReleaseGame"
					ReleaseConfig()
					defines{"_RELEASEGAME"}


		group "Engine/Runtime"
			project "CodeGen"
				includedirs{ "Source/Runtime/CodeGen/", "Source/ThirdParty", "Source/Runtime/Core/", }
				links{ "Core", }
				SetCPPProjectConfig("SharedLib", "Source/Runtime/CodeGen/", {"_CODEGEN", })
				filter "configurations:DevelopEngine"
					DebugConfig()
					defines{"_DEVELOPENGINE"}
				filter "configurations:DevelopGame"
					ConfirmConfig()
					defines{"_DEVELOPGAME"}
				filter "configurations:ConfirmGame"
					ConfirmConfig()
					defines{"_CONFIRMGAME"}
				filter "configurations:ReleaseGame"
					ReleaseConfig()
					defines{"_RELEASEGAME"}


			project "Core"
				includedirs{ "Source/Runtime/Core/", "Source/ThirdParty", }
				links{ }
				SetCPPProjectConfig("StaticLib", "Source/Runtime/Core/", {"_CORE", })
				filter "configurations:DevelopEngine"
					DebugConfig()
					defines{"_DEVELOPENGINE"}
				filter "configurations:DevelopGame"
					ConfirmConfig()
					defines{"_DEVELOPGAME"}
				filter "configurations:ConfirmGame"
					ConfirmConfig()
					defines{"_CONFIRMGAME"}
				filter "configurations:ReleaseGame"
					ReleaseConfig()
					defines{"_RELEASEGAME"}




