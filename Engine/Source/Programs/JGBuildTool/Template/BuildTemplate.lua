local GEN_PROJECT_FILE_PATH = "Temp/ProjectFiles/"
local BIN_PATH = "Bin/%{cfg.buildcfg}/"
local OBJECT_PATH = "Temp/Obj/%{cfg.buildcfg}/"


function DebugConfig()
    symbols       "On"
    optimize      "Off"
    defines       {"_DEBUG", "_PROFILE"}
    cppdialect    "C++20"
    staticruntime "off"
    runtime       "Debug"
end

function ConfirmConfig()
    optimize        "Full" 
    defines         {"_RELEASE", "NDEBUG", "_PROFILE"}
    cppdialect      "C++20"
    staticruntime   "off"
    runtime         "Release"
end

function ReleaseConfig()
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
        if defined ~= nil then
            defines {defined}
        end
        -- file
        files {
            path .. "**.h",
            path .. "**.cpp",
            path .. "**.c",
        }
    end

    group "Engine"