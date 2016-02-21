include "../../3rdParty/googletest/premake5.lua"

project "Test"
   kind "StaticLib"
   language "C++"
   targetdir "../../Bin/%{cfg.buildcfg}"
   links { "GoogleTest" }
   includedirs { "Include", "../../3rdParty/googletest/include" }

   files { "Include/**.h", "Source/**.c", "Source/**.cpp", "Tests/**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"