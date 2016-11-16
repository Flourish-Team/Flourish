project "GoogleTest"
   kind "StaticLib"
   language "C++"
   targetdir "../../Bin/%{cfg.buildcfg}"
   defines { "GTEST_HAS_PTHREAD=0" }

   --CB: using the unity build file
   files { "include/gtest/**.h", "scr/**.h", "src/gtest-all.cc" }
   includedirs { "include", "." }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   configuration "macosx"
            linkoptions  { "-std=c++11", "-stdlib=libc++" }
            buildoptions { "-std=c++11", "-stdlib=libc++" }