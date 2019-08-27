workspace "JGGame"
   architecture "x64"
   startproject "EntryPoint"
   configurations 
   {
   -- All Engine Debug
      "DevelopEngine",
   -- Engine is DevelopEngineOnRelease CustomGame is Debug
     "DevelopGame",
   -- All Engine and CustomGame is Release but DebugInfo
     "DevelopGameOnRelease",
     -- All Release
     "Release"
   }


   function DebugConfig()
     symbols  "On"
     optimize "Off"
     defines {"_DEBUG"}
   end

   function ReleaseOnDebugInfoConfig()
     optimize "Full" 
     defines {"_RELEASE_ONDEBUG"}
   end

   function ReleaseConfig()
     optimize "Full" 
     defines {"_RELEASE"}
   end
   function SetDllPath()
      -- 나중에 추가로 Game 개발시에 다른 경로로 Dll 복사
      --     postbuildcommands("{COPY} %{cfg.buildtarget.directory}%{cfg.buildtarget.name} -- path --")
      --     debugenvs 
      debugenvs {"PATH=%{wks.location}Build\\dlls_libs\\%{cfg.buildcfg}\\;%PATH%;"}
   end

   function SetEngineConfiguarations()
      filter "configurations:DevelopEngine"
        DebugConfig()
        defines("_DEVELOP_ENGINE")
     filter "configurations:DevelopGame"
        ReleaseOnDebugInfoConfig()
        defines("_DEVELOP_GAME")
     filter "configurations:DevelopGameOnRelease"
        ReleaseOnDebugInfoConfig()
        defines("_DEVELOP_GAME_ONRELEASE")
     filter "configurations:Release"
        ReleaseConfig()
        defines("_GAME_RELEASE")
   end

   function SetGameConfiguarations()
      filter "configurations:DevelopEngine"
        DebugConfig()
        defines("_DEVELOP_ENGINE")
     filter "configurations:DevelopGame"
        DebugConfig()
        defines("_DEVELOP_GAME")
     filter "configurations:DevelopGameOnRelease"
        ReleaseOnDebugInfoConfig()
        defines("_DEVELOP_GAME_ONRELEASE")
     filter "configurations:Release"
        ReleaseConfig()
        defines("_GAME_RELEASE")
   end

   


  os.mkdir("Bin")
  os.mkdir("Config")
  os.mkdir("Contents")
  os.mkdir("Temp")
  os.mkdir("Build")


group "Engine"
     -- Engine Core Project Setting
   project "Common"
      location "Build"
      kind "StaticLib"
      language "C++"
      targetdir ("./Build/dlls_libs/%{cfg.buildcfg}/")
      objdir("./Build/obj/%{cfg.buildcfg}/")

      -- precompiled header
      pchheader ("pch.h")
      pchsource ("JGEngine/Common/pch.cpp")
      -- files
      files{
         "JGEngine/Common/**.h",
         "JGEngine/Common/**.cpp"
      }
      -- add include dir
      includedirs{
         "JGEngine/Library",
         "JGEngine/Common"
      }
      -- solution virual paths
         vpaths{
         ["*"] = {"JGEngine/Common/**.h", "JGEngine/Common/**.cpp"}
      }
      SetEngineConfiguarations()
      



   project "JGEngine"
      location "Build"
      kind "SharedLib"
      language "C++"
      targetdir ("./Build/dlls_libs/%{cfg.buildcfg}/")
      objdir("./Build/obj/%{cfg.buildcfg}/")
      -- precompiled header
      pchheader ("pch.h")
      pchsource ("JGEngine/Application/pch.cpp")
      -- defines
      defines("JGENGINE_EXPORTS")
      -- files
      files{
         "JGEngine/Application/**.h",
         "JGEngine/Application/**.cpp"
      }
       -- add include dir
      includedirs{
         "JGEngine/Library",
         "JGEngine/Common",
         "JGEngine/Application",
         "JGEngine/Core/GameFrameWork",
         "JGEngine/Core/PhysicsEngine",
         "JGEngine/Core/InputEngine",
         "JGEngine/Core/RenderEngine",
         "JGEngine/Core/SoundEngine",
      }
      -- linkProject
      links {
         "Common",
         "RenderEngine",
         "PhysicsEngine",
         "SoundEngine",
         "InputEngine",
         "GameFrameWork"
      }
      -- solution virual paths
      vpaths{
         ["*"] = {"JGEngine/Application/**.h", "JGEngine/Application/**.cpp"}
      }
      filter "files:JGEngine/Application/GUI/EditorGUI/ImGui/imgui.cpp"
         flags("NoPCH")
      filter "files:JGEngine/Application/GUI/EditorGUI/ImGui/imgui_demo.cpp"
         flags("NoPCH")
      filter "files:JGEngine/Application/GUI/EditorGUI/ImGui/imgui_impl_win32.cpp"
         flags("NoPCH")
      filter "files:JGEngine/Application/GUI/EditorGUI/ImGui/imgui_widgets.cpp"
         flags("NoPCH")
      filter "files:JGEngine/Application/GUI/EditorGUI/ImGui/imgui_draw.cpp"
         flags("NoPCH")
      SetEngineConfiguarations()

      
   project  "EntryPoint"
      location "Build"
      kind "WindowedApp"
      language "C++"
      targetdir ("./Build/exe/%{cfg.buildcfg}/")
      objdir("./Build/exe/%{cfg.buildcfg}/")
      SetDllPath()
      files{
         "JGEngine/EntryPoint/**.h",
         "JGEngine/EntryPoint/**.cpp"
      }
      includedirs{
         "JGEngine/Library",
         "JGEngine/Common",
         "JGEngine/Application",
      }
      -- linkProject
      links {
         "Common",
         "JGEngine"
      }
      SetEngineConfiguarations()

   group "Engine/Core"
      project "RenderEngine"
         location "Build"
         kind "SharedLib"
         language "C++"
         targetdir ("./Build/dlls_libs/%{cfg.buildcfg}/")
         objdir("./Build/obj/%{cfg.buildcfg}/")
         -- precompiled header
         pchheader ("pch.h")
         pchsource ("JGEngine/Core/RenderEngine/pch.cpp")
         -- defines
         defines("RENDERENGINE_EXPORTS")
         -- files
         files{
            "JGEngine/Core/RenderEngine/**.h",
            "JGEngine/Core/RenderEngine/**.cpp"
         }
          -- add include dir
         includedirs{
            "JGEngine/Library",
            "JGEngine/Common",
            "JGEngine/Core/RenderEngine"
         }
         -- linkProject
         links {
            "Common"
         }
         -- solution virual paths
         vpaths{
            ["*"] = {"JGEngine/Core/RenderEngine/**.h", "JGEngine/Core/RenderEngine/**.cpp"}
         }
         SetEngineConfiguarations()


      project "InputEngine"
         location "Build"
         kind "SharedLib"
         language "C++"
         targetdir ("./Build/dlls_libs/%{cfg.buildcfg}/")
         objdir("./Build/obj/%{cfg.buildcfg}/")
         -- precompiled header
         pchheader ("pch.h")
         pchsource ("JGEngine/Core/InputEngine/pch.cpp")
         -- defines
         defines("INPUTENGINE_EXPORTS")
         -- files
         files{
            "JGEngine/Core/InputEngine/**.h",
            "JGEngine/Core/InputEngine/**.cpp"
         }
          -- add include dir
         includedirs{
            "JGEngine/Library",
            "JGEngine/Common",
            "JGEngine/Core/InputEngine"
         }
         -- linkProject
         links {
            "Common"
         }
         -- solution virual paths
         vpaths{
            ["*"] = {"JGEngine/Core/InputEngine/**.h", "JGEngine/Core/InputEngine/**.cpp"}
         }
         SetEngineConfiguarations()
         

      project "PhysicsEngine"
         location "Build"
         kind "SharedLib"
         language "C++"
         targetdir ("./Build/dlls_libs/%{cfg.buildcfg}/")
         objdir("./Build/obj/%{cfg.buildcfg}/")
         -- precompiled header
         pchheader ("pch.h")
         pchsource ("JGEngine/Core/PhysicsEngine/pch.cpp")
         -- defines
         defines("PHYSICSENGINE_EXPORTS")
         -- files
         files{
            "JGEngine/Core/PhysicsEngine/**.h",
            "JGEngine/Core/PhysicsEngine/**.cpp"
         }
          -- add include dir
         includedirs{
            "JGEngine/Library",
            "JGEngine/Common",
            "JGEngine/Core/PhysicsEngine"
         }
         -- linkProject
         links {
            "Common"
         }
         -- solution virual paths
         vpaths{
            ["*"] = {"JGEngine/Core/PhysicsEngine/**.h", "JGEngine/Core/PhysicsEngine/**.cpp"}
         }
         SetEngineConfiguarations()

      project "SoundEngine"
         location "Build"
         kind "SharedLib"
         language "C++"
         targetdir ("./Build/dlls_libs/%{cfg.buildcfg}/")
         objdir("./Build/obj/%{cfg.buildcfg}/")
         -- precompiled header
         pchheader ("pch.h")
         pchsource ("JGEngine/Core/SoundEngine/pch.cpp")
         -- defines
         defines("SOUNDENGINE_EXPORTS")
         -- files
         files{
            "JGEngine/Core/SoundEngine/**.h",
            "JGEngine/Core/SoundEngine/**.cpp"
         }
          -- add include dir
         includedirs{
            "JGEngine/Library",
            "JGEngine/Common",
            "JGEngine/Core/SoundEngine"
         }
         -- linkProject
         links {
            "Common"
         }
         -- solution virual paths
         vpaths{
            ["*"] = {"JGEngine/Core/SoundEngine/**.h", "JGEngine/Core/SoundEngine/**.cpp"}
         }
         SetEngineConfiguarations()

      project "GameFrameWork"
         location "Build"
         kind "SharedLib"
         language "C++"
         targetdir ("./Build/dlls_libs/%{cfg.buildcfg}/")
         objdir("./Build/obj/%{cfg.buildcfg}/")
         -- precompiled header
         pchheader ("pch.h")
         pchsource ("JGEngine/Core/GameFrameWork/pch.cpp")
         -- defines
         defines("GAMEFRAMEWORK_EXPORTS")
         -- files
         files{
            "JGEngine/Core/GameFrameWork/**.h",
            "JGEngine/Core/GameFrameWork/**.cpp"
         }
          -- add include dir
         includedirs{
            "JGEngine/Library",
            "JGEngine/Common",
            "JGEngine/Core/GameFrameWork"
         }
         -- linkProject
         links {
            "Common"
         }
         -- solution virual paths
         vpaths{
            ["*"] = {"JGEngine/Core/GameFrameWork/**.h", "JGEngine/Core/GameFrameWork/**.cpp"}
         }
         SetEngineConfiguarations()
      

group "%{wks.name}"


group ""












  





