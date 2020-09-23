
call JGEngineMake.exe vs2019 --file=JGEngineGenerator.lua 
copy Source\ThirdParty\dll_lib\*.dll Build
copy Source\ThirdParty\dll_lib\*.lib Build
PAUSE