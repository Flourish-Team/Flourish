project "TestProject"
   kind "ConsoleApp"
   language "C++"
   targetdir "../Bin/%{cfg.buildcfg}"
   includedirs 
   { 
      "../Libs/Core/Include",
      "../Libs/Core/Tests",
      "../Libs/Test/Include",
      "../3rdParty/googletest/include" 
   }

   links { "Core", "Test", "GoogleTest" }

   files { 
      "Source/**.cpp",
      "../Libs/**/Tests/**.cpp",
      "../Libs/**/Tests/**.h"
   }

   defines { "FL_RUNNING_TESTS" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   configuration "macosx"
            linkoptions  { "-std=c++11", "-stdlib=libc++" }
            buildoptions { "-std=c++11", "-stdlib=libc++" }