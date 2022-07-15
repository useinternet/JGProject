
local PCH_HEADER = "pch.h"
local PCH_HEADER_PATH = "Source/Core/pch.h"
local PCH_CPP_PATH    = "Source/Core/pch.cpp"



function DebugConfig()
    symbols  "On"
    optimize "Off"
    defines {"_DEBUG"}
    cppdialect "C++17"
    staticruntime "off"
    runtime "Debug"
end


function ReleaseConfig()
    optimize "Full" 
    defines {"_RELEASE", "NDEBUG"}
    cppdialect "C++17"
    staticruntime "off"
    runtime "Release"
end



workspace "JGEngine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

    function SetStaticLibConfig(path, defined)
        location  "Build"
        kind "StaticLib"
        language "C++"
        cppmodules "enabled"
        debugdir  ("Bin/%{cfg.buildcfg}/")
        targetdir ("Bin/%{cfg.buildcfg}/")
        objdir("Build/Obj/%{cfg.buildcfg}/")
        if defined ~= nil then
            defines {defined}
        end


        -- file
        files {
            path .. "**.h",
            path .. "**.cpp",
            path .. "**.c",
            PCH_HEADER_PATH,
            PCH_CPP_PATH,
        }

        filter "files:**.ixx"
        compileas "Module"
  
        filter "files:**.cpp"
        compileas "ModulePartition"
  
        filter "toolset:gcc
          toolset "gcc11"

        filter "configurations:Debug"
            DebugConfig()
        filter "configurations:Release"
            ReleaseConfig()
    end

    function SetSharedLibConfig(path, bridgePath, defined)
        location  "Build"
        kind "SharedLib"
        language "C++"
        debugdir  ("Bin/%{cfg.buildcfg}/")
        targetdir ("Bin/%{cfg.buildcfg}/")
        objdir("Build/Obj/%{cfg.buildcfg}/")
        if defined ~= nil then
            defines {defined}
        end

        -- file
        files {
            path .. "**.h",
            path .. "**.cpp",
            path .. "**.c",
            bridgePath .. "**.h",
            bridgePath .. "**.cpp",
            bridgePath .. "**.c",
            PCH_HEADER_PATH,
            PCH_CPP_PATH,
        }
        filter "configurations:Debug"
            DebugConfig()
        filter "configurations:Release"
            ReleaseConfig()

    end

    function SetConsoleAppConfig(path, defined)
        location  "Build"
        kind "ConsoleApp"
        language "C++"
        debugdir  ("Bin/%{cfg.buildcfg}/")
        targetdir ("Bin/%{cfg.buildcfg}/")
        objdir("Build/Obj/%{cfg.buildcfg}/")
        if defined ~= nil then
            defines {defined}
        end
        -- file
        files {
            path .. "**.h",
            path .. "**.cpp",
            path .. "**.ico",
            path .. "**.rc",
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
                    "ThirdParty/physx/"
                }
                links {
                    "Imgui",
                    "stb",
                    "zlib",
                    "Publish/%{cfg.buildcfg}/dxcompiler",
                    "Publish/%{cfg.buildcfg}/assimp-vc142-mt",
                }
                pchheader (PCH_HEADER)
                pchsource (PCH_CPP_PATH)

                postbuildcommands {"copy /b /y \"..\\Publish/%{cfg.buildcfg}\\*.lib\" \"..\\Bin\\%{cfg.buildcfg}\""}
                postbuildcommands {"copy /b /y \"..\\Publish/%{cfg.buildcfg}\\*.dll\" \"..\\Bin\\%{cfg.buildcfg}\""}
                SetStaticLibConfig("Source/Core/", "JG_EDITOR")
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
                    "Core", 
                    "Game",
                }
                SetConsoleAppConfig("Source/Editor/", "JG_EDITOR")
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
                SetStaticLibConfig("Source/Game/", "JG_EDITOR")
        group "Engine/Develop"
            project "Develop"
                includedirs{
                    "Source/Core/",
                    "ThirdParty",
                }
                pchheader (PCH_HEADER)
                pchsource (PCH_CPP_PATH)
                links {
                    "Core"
                }
                SetConsoleAppConfig("Source/Develop/", "JG_Develop")
        group "User/SandBox"
            project "SandBox_Game"
                includedirs{
                    "Source/Core/",
                    "Source/Game/",
                    "Source/Bridge/Game/",
                    "ThirdParty",
                }
                pchheader (PCH_HEADER)
                pchsource (PCH_CPP_PATH)
                links{
                    "Core", "Game"
                }
                SetSharedLibConfig("../GameProject/Project_C/Asset/Cpp/Game/", "Source/Bridge/Game/", "GAME_DLL_EXPORT")
            project "SandBox_Editor"
                includedirs{
                    "Source/Core/",
                    "Source/Editor/",
                    "Source/Game/",
                    "Source/Bridge/Editor/",
                    "ThirdParty",
                }
                pchheader (PCH_HEADER)
                pchsource (PCH_CPP_PATH)
                links{
                    "Core", "Game", "Editor"
                }
                SetSharedLibConfig("../GameProject/Project_C/Asset/Cpp/Editor/", "Source/Bridge/Editor/", "EDITOR_DLL_EXPORT")
            
            
            
    group "ThirdParty"
        project "Imgui"
            SetStaticLibConfig("ThirdParty/Imgui/")
        project "stb"
            SetStaticLibConfig("ThirdParty/stb/")
        project "zlib"
            SetStaticLibConfig("ThirdParty/zlib/")


        

    







