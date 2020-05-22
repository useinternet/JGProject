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
   --function 
   

   function DebugConfig()
     symbols  "On"
     optimize "Off"
     defines {"_DEBUG"}
     cppdialect "C++17"
   end

   function ReleaseOnDebugInfoConfig()
     optimize "Full" 
     cppdialect "C++17"
     defines {"_RELEASE_ONDEBUG"}
   end

   function ReleaseConfig()
     optimize "Full" 
     cppdialect "C++17"
     defines {"_RELEASE"}
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

  os.mkdir("Bin")
  os.mkdir("Config")
  os.mkdir("Contents")
  os.mkdir("Source")
  os.mkdir("Build")


group "Engine"
     -- Engine Core Project Setting
   project "Common"
      location "Build"
      kind "StaticLib"
      language "C++"
     --cppdialect "std:c++17"
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
     --cppdialect "std:c++17"
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
         "GraphicsEngine",
         "GameFrameWork"
      }
      -- solution virual paths
      vpaths{
         ["*"] = {"JGEngine/Application/**.h", "JGEngine/Application/**.cpp"}
      }
      
      SetEngineConfiguarations()

      
   project  "EntryPoint"
      location "Build"
      kind "WindowedApp"
      language "C++"
     --cppdialect "std:c++17"
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
        --cppdialect "std:c++17"
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
         EngineShaderedLibConfig(
            "INPUTENGINE_EXPORTS", 
            "JGEngine/Core/InputEngine/"
         )

      project "PhysicsEngine"

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
         EngineShaderedLibConfig(
            "PHYSICSENGINE_EXPORTS", 
            "JGEngine/Core/PhysicsEngine/"
         )

      project "SoundEngine"
         includedirs{
            "JGEngine/Library",
            "JGEngine/Common",
            "JGEngine/Core/SoundEngine"
         }
         -- linkProject
         links {
            "Common"
         }
         EngineShaderedLibConfig(
            "SOUNDENGINE_EXPORTS", 
            "JGEngine/Core/SoundEngine/"
         )
          -- add include dir
   

      project "GameFrameWork"

         includedirs{
            "JGEngine/Library",
            "JGEngine/Common",
            "JGEngine/Core/GameFrameWork"
         }
         links {
            "Common"
         }
         EngineShaderedLibConfig(
            "GAMEFRAMEWORK_EXPORTS", 
            "JGEngine/Core/GameFrameWork/"
         )
        


      -- GraphicsEngine --
      group "Engine/Core/GraphicsEngine"
         project "GraphicsEngine"

          -- add include dir
            includedirs{
               "JGEngine/Library",
               "JGEngine/Common",
               "JGEngine/Core/GraphicsEngine/Main/"
            }
            -- linkProject
            links {
               "Common"
            }
            EngineShaderedLibConfig(
               "GRAPHICSENGINE_EXPORTS", 
               "JGEngine/Core/GraphicsEngine/Main/"
            )

         group "Engine/Core/GraphicsEngine/GPU_API"
            project "DirectX12"
 
               includedirs{
                  "JGEngine/Library",
                  "JGEngine/Common",
                  "JGEngine/Core/GraphicsEngine/GPU_API/DirectX12"
                  "JGEngine/Core/GraphicsEngine/Main"
               }
               links {
                  "Common",
                  "GraphicsEngine"
               }
               EngineShaderedLibConfig(
                  "DIRECTX12_EXPORTS", 
                  "JGEngine/Core/GraphicsEngine/GPU_API/DirectX12/"
               )
            project "DirectX11"

               includedirs{
                  "JGEngine/Library",
                  "JGEngine/Common",
                  "JGEngine/Core/GraphicsEngine/GPU_API/DirectX11"
                  "JGEngine/Core/GraphicsEngine/Main"
               }
               links {
                  "Common",
                  "GraphicsEngine"
               }
               EngineShaderedLibConfig(
                  "DIRECTX11_EXPORTS", 
                  "JGEngine/Core/GraphicsEngine/GPU_API/DirectX11/"
               )

            project "OpenGL"

               includedirs{
                  "JGEngine/Library",
                  "JGEngine/Common",
                  "JGEngine/Core/GraphicsEngine/GPU_API/OpenGL"
                  "JGEngine/Core/GraphicsEngine/Main"
               }
               links {
                  "Common",
                  "GraphicsEngine"
               }
               EngineShaderedLibConfig(
                  "OPENGL_EXPORTS", 
                  "JGEngine/Core/GraphicsEngine/GPU_API/OpenGL/"
               )
      

               

group "Game"
   project "JGGame"
   location "Build"
   kind "SharedLib"
   language "C++"
  --cppdialect "std:c++17"
   targetdir ("./Build/dlls_libs/%{cfg.buildcfg}/")
   objdir("./Build/obj/%{cfg.buildcfg}/")
   -- defines
   defines("JGGAME_EXPORTS")
   --
   -- files
   files{
      "Source/**.h",
      "Source/**.cpp"
   }

   -- add include dir
   includedirs{
      "JGEngine/Library",
      "JGEngine/Common",
      "JGEngine/Core/GameFrameWork"
   }
   -- linkProject
   links {
      "Common",
      "GameFrameWork"
   }
   vpaths{
      ["*"] = {"Source/**.h", "Source/**.cpp"}
   }
   SetGameConfiguarations()


group "Editor"
   project  "JGEditor"

      includedirs{
         "JGEngine/Library",
         "JGEngine/Common",
         "JGEngine/Application",
         "JGEngine/Editor"
      }
      links {
         "Common",
         "JGEngine"
      }
      EngineShaderedLibConfig(
         "JGEDITOR_EXPORTS", 
         "JGEngine/Editor/"
      )
group ""












  





