
local PCH_HEADER = "pch.h"
local PCH_HEADER_PATH = "Source/Core/pch.h"
local PCH_CPP_PATH    = "Source/Core/pch.cpp"


function DebugConfig()
    symbols  "On"
    optimize "Off"
    defines {"_DEBUG"}
    cppdialect "C++17"
  end


function ReleaseConfig()
    optimize "Full" 
    defines {"_RELEASE"}
    cppdialect "C++17"
end

workspace "JGEngine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

    function SetStaticLibConfig(path)
        location  "Build"
        kind "StaticLib"
        language "C++"
        debugdir  ("Bin/%{cfg.buildcfg}/")
        targetdir ("Bin/%{cfg.buildcfg}/")
        objdir("Build/Obj/%{cfg.buildcfg}/")



        -- file
        files {
            path .. "**.h",
            path .. "**.cpp",
            path .. "**.c",
            PCH_HEADER_PATH,
            PCH_CPP_PATH,
        }



        filter "configurations:Debug"
            DebugConfig()
        filter "configurations:Release"
            ReleaseConfig()
    end

    function SetSharedLibConfig(defined, path)
        location  "Build"
        kind "SharedLib"
        language "C++"
        debugdir  ("Bin/%{cfg.buildcfg}/")
        targetdir ("Bin/%{cfg.buildcfg}/")
        objdir("Build/Obj/%{cfg.buildcfg}/")
        defines(defined)

        -- file
        files {
            path .. "**.h",
            path .. "**.cpp",
            path .. "**.c",
            PCH_HEADER_PATH,
            PCH_CPP_PATH,
        }

        filter "configurations:Debug"
            DebugConfig()
        filter "configurations:Release"
            ReleaseConfig()

    end

    function SetConsoleAppConfig(path)
        location  "Build"
        kind "ConsoleApp"
        language "C++"
        debugdir  ("Bin/%{cfg.buildcfg}/")
        targetdir ("Bin/%{cfg.buildcfg}/")
        objdir("Build/Obj/%{cfg.buildcfg}/")
        -- file
        files {
            path .. "**.h",
            path .. "**.cpp",
            PCH_HEADER_PATH,
            PCH_CPP_PATH,
        }

        filter "configurations:Debug"
            DebugConfig()
        filter "configurations:Release"
            ReleaseConfig()
    end










    os.mkdir("Bin");
    os.mkdir("Build");



    group "Engine"
        group "Engine/Core"
            project "Core"
                includedirs{
                    "Source/Core/",
                    "ThirdParty/",
                }
                links {
                    "Imgui",
                    "stb",
                    "Publish/%{cfg.buildcfg}/assimp-vc142-mt",
                }
                pchheader (PCH_HEADER)
                pchsource (PCH_CPP_PATH)

                postbuildcommands {"copy /b /y \"..\\Publish/Debug\\*.lib\" \"..\\Bin\\%{cfg.buildcfg}\""}
                postbuildcommands {"copy /b /y \"..\\Publish/Debug\\*.dll\" \"..\\Bin\\%{cfg.buildcfg}\""}
                SetStaticLibConfig("Source/Core/")
        group "Engine/Editor"
            project "Editor"
                includedirs{
                    "Source/Core/",
                    "Source/Editor/",
                    "Source/Game/",
                    "ThirdParty/",
                }
                pchheader (PCH_HEADER)
                pchsource (PCH_CPP_PATH)
                links {
                    "Core", "Game",
                }
                SetConsoleAppConfig("Source/Editor/")
        group "Engine/Game"
            project "Game"
                includedirs{
                    "Source/Core/",
                    "Source/Game/",
                    "ThirdParty",
                }
                pchheader (PCH_HEADER)
                pchsource (PCH_CPP_PATH)
                links{
                    "Core"
                }
                SetStaticLibConfig("Source/Game/")
    group "ThirdParty"
        project "Imgui"
            SetStaticLibConfig("ThirdParty/Imgui/")
        project "stb"
            SetStaticLibConfig("ThirdParty/stb/")


        

    







