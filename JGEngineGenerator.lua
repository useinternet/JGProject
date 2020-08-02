
workspace "JGEngine"
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
   --function 
   function DebugConfig()
     symbols  "On"
     optimize "Off"
     defines {"_DEBUG"}
     cppdialect "C++17"
   end

   function ReleaseOnDebugConfig()
     optimize "Full" 
  
     defines {"_RELEASE_ONDEBUG"}
     cppdialect "C++17"
   end

   function ReleaseConfig()
     optimize "Full" 
     defines {"_RELEASE"}
     cppdialect "C++17"
   end
   
   function SetDllPath()
      -- 나중에 추가로 Game 개발시에 다른 경로로 Dll 복사
      --     postbuildcommands("{COPY} %{cfg.buildtarget.directory}%{cfg.buildtarget.name} -- path --")
      --     debugenvs 
      debugenvs {"PATH=%{wks.location}Build\\dlls_libs\\%{cfg.buildcfg}\\;%PATH%;"}
   end
--
   function SetEngineConfiguarations()
      filter "configurations:DevelopEngine"
        DebugConfig()
        defines("_DEVELOP_ENGINE")
     filter "configurations:DevelopGame"
        ReleaseOnDebugConfig()
        defines("_DEVELOP_GAME")
     filter "configurations:DevelopGameOnRelease"
        ReleaseOnDebugConfig()
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
        ReleaseOnDebugConfig()
        defines("_DEVELOP_GAME_ONRELEASE")
     filter "configurations:Release"
        ReleaseConfig()
        defines("_GAME_RELEASE")
   end

   
   function EngineShaderedLibConfig(defined, path)
   
      location "Build"
      kind "SharedLib"
      language "C++"
     --cppdialect "std:c++17"
      targetdir ("./Build/dlls_libs/%{cfg.buildcfg}/")
      objdir("./Build/obj/%{cfg.buildcfg}/")
      -- precompiled header
      pchheader ("pch.h")
      pchsource (path .. "pch.cpp")
      -- defines
      defines(defined)
      -- files
      files{
         path .. "**.h",
         path .. "**.cpp"
      }
      -- solution virual paths
      vpaths{
         ["*"] = {path .. "**.h", path .. "**.cpp"}
      }
      
      SetEngineConfiguarations()

   end

   
   function EngineStaticLibConfig(path)
   
      location "Build"
      kind "StaticLib"
      language "C++"
      targetdir ("./Build/dlls_libs/%{cfg.buildcfg}/")
      objdir("./Build/obj/%{cfg.buildcfg}/")
      -- precompiled header
      pchheader ("pch.h")
      pchsource (path .. "pch.cpp")
      -- files
      files{
         path .. "**.h",
         path .. "**.cpp"
      }
      -- solution virual paths
      vpaths{
         ["*"] = {path .. "**.h", path .. "**.cpp"}
      }
      
      SetEngineConfiguarations()

   end




  os.mkdir("Bin")
  os.mkdir("Build")





group "Engine"
     -- Engine Core Project Setting
   project "Common"
      includedirs{
         "Source/Common/",
      }
      -- linkProject
      links {
         -- ...
      }
      EngineStaticLibConfig("Source/Common/")

   project "Application"
      includedirs{
         "Source/Common/",
         "Source/Application/Main/"
      }
      -- linkProject
      links {
         "Common",
      }
      EngineShaderedLibConfig("APPLICATION_EXPORT", "Source/Application/Main/")

   project  "EntryPoint"
      location "Build"
      kind "WindowedApp"
      language "C++"
     --cppdialect "std:c++17"
      targetdir ("./Build/exe/%{cfg.buildcfg}/")
      objdir("./Build/exe/%{cfg.buildcfg}/")
      SetDllPath()
      files{
         "Source/EntryPoint/**.h",
         "Source/EntryPoint/**.cpp"
      }
      includedirs{
         "Source/Common/",
         "Source/Application/Main/",
      }
      -- linkProject
      links {
         "Common",
         "Application"
      }
      SetEngineConfiguarations()


      group "Engine/Core"
         project "PhysicsEngine"
            includedirs{
             "Source/Core/PhysicsEngine/Main",
             "Source/Common/"
            }
            -- linkProject
            links {
               "Common",
            }
            EngineShaderedLibConfig("PHYSICSENGINE_EXPORT", "Source/Core/PhysicsEngine/Main/")

         project "SoundEngine"
            includedirs{
             "Source/Core/SoundEngine/Main",
             "Source/Common/"
            }
            -- linkProject
            links {
               "Common",
            }
            EngineShaderedLibConfig("SOUNDENGINE_EXPORT", "Source/Core/SoundEngine/Main/")

         project "InputEngine"
            includedirs{
             "Source/Core/InputEngine/Main",
             "Source/Common/"
            }
            -- linkProject
            links {
               "Common",
            }
            EngineShaderedLibConfig("INPUTENGINE_EXPORT", "Source/Core/InputEngine/Main/")
 
         project "GameFrameWork"
            includedirs{
             "Source/Core/GameFrameWork/Main",
             "Source/Common/"
            }
            -- linkProject
            links {
               "Common",
            }
            EngineShaderedLibConfig("GAMEFRAMEWORK_EXPORT", "Source/Core/GameFrameWork/Main/")

         project "GUI"
            includedirs{
             "Source/Core/GUI/Main",
             "Source/Core/GraphicsEngine/Main",
             "Source/Core/InputEngine/Main",
             "Source/Common/"
            }
            -- linkProject
            links {
               "Common",
               "GraphicsEngine",
               "InputEngine"
            }
            EngineShaderedLibConfig("GUI_EXPORT", "Source/Core/GUI/Main/")

         group "Engine/Core/GraphicsEngine"
            project "GraphicsEngine"
               includedirs{
                  "Source/Core/GraphicsEngine/Main",
                  "Source/Common/"
                  }
               -- linkProject
               links {
                  "Common",
               }
               EngineShaderedLibConfig("GRAPHICSENGINE_EXPORT", "Source/Core/GraphicsEngine/Main/")

            group "Engine/Core/GraphicsEngine/GPU_API"
               project "DirectX12"
                  includedirs{
                     "Source/Core/GraphicsEngine/GPU_API/DirectX12/Main/",
                     "Source/Core/GraphicsEngine/Main/Interface/",
                     "Source/Common/"
                  }
                  -- linkProject
                  links {
                     "Common",
                     "GraphicsEngine"
                  }
                  EngineShaderedLibConfig("DX12_EXPORT", "Source/Core/GraphicsEngine/GPU_API/DirectX12/Main/")
 
               project "DirectX11"
                  includedirs{
                     "Source/Core/GraphicsEngine/GPU_API/DirectX11/Main/",
                     "Source/Core/GraphicsEngine/Main/Interface/",
                     "Source/Common/"
                  }
                  -- linkProject
                  links {
                     "Common",
                     "GraphicsEngine"
                  }
                  EngineShaderedLibConfig("DX11_EXPORT", "Source/Core/GraphicsEngine/GPU_API/DirectX11/Main/")

               project "OpenGL"
                  includedirs{
                     "Source/Core/GraphicsEngine/GPU_API/OpenGL/Main/",
                     "Source/Core/GraphicsEngine/Main/Interface/",
                     "Source/Common/"
                  }
                  -- linkProject
                  links {
                     "Common",
                     "GraphicsEngine"
                  }
                  EngineShaderedLibConfig("OPENGL_EXPORT", "Source/Core/GraphicsEngine/GPU_API/OpenGL/Main/")
      
      

         


group "Editor"
   project "JGEditor"
      includedirs{
         "Source/Common/"
         "Source/Core/GUI/Main",
         "Source/Core/GraphicsEngine/Main",
      }
            -- linkProject
      links {
         "Common",
         "GUI",
         "GraphicsEngine"
      }
      EngineShaderedLibConfig("JGEDITOR_EXPORT", "Source/User/Editor/Main/")


group "UserGame"
   project "Game"
      includedirs{

      }
      -- linkProject
      links {
         "Common",
         "GameFrameWork",
      }
      EngineShaderedLibConfig("GAME_EXPORT", "Source/User/Game/")


group "UnitTest"
   project  "UTWindows"
      location "Build"
      kind "WindowedApp"
      language "C++"
      cppdialect "C++17"
      targetdir ("./Build/UnitTestEXE/%{cfg.buildcfg}/")
      objdir("./Build/UnitTestEXE/%{cfg.buildcfg}/")
      SetDllPath()
      files{
         "UnitTest/UTWindows/**.h",
         "UnitTest/UTWindows/**.cpp",
         "UnitTest/UTWindows/**.ico",
         "UnitTest/UTWindows/**.rc"
      }
      includedirs{
         "Source/Common",
         "UnitTest/UTWindows"
      }
      vpaths{
         ["*"] = {"UnitTest/UTWindows/**.h", "UnitTest/UTWindows/**.cpp", "UnitTest/UTWindows/**.ico", "UnitTest/UTWindows/**.rc"}
      }
      -- linkProject
      links {
         "Common",
      }
      SetEngineConfiguarations()
   project  "Console"
      location "Build"
      kind "ConsoleApp"
      language "C++"
      cppdialect "C++17"
      targetdir ("./Build/UnitTestEXE/%{cfg.buildcfg}/")
      objdir("./Build/UnitTestEXE/%{cfg.buildcfg}/")
      SetDllPath()
      files{
         "UnitTest/UTConsole/**.h",
         "UnitTest/UTConsole/**.cpp"

      }
      includedirs{
         "Source/Common",
         "UnitTest/UTConsole"
      }
      vpaths{
         ["*"] = {"UnitTest/UTConsole/**.h", "UnitTest/UTConsole/**.cpp"}
      }
      -- linkProject
      links {
         "Common",
      }
      SetEngineConfiguarations()

group ""








  





