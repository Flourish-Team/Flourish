project "GoogleTest"
   kind "StaticLib"
   language "C++"
   systemversion "latest"
   targetdir "../../Bin/%{cfg.buildcfg}"

   --CB: using the unity build file
   files { "include/gtest/**.h", "scr/**.h", "src/gtest-all.cc" }
   includedirs { "include", "." }

   configuration "macosx"
      linkoptions  { "-std=c++11", "-stdlib=libc++" }
      buildoptions { "-std=c++11", "-stdlib=libc++" }

   configuration "windows"
      defines { "GTEST_HAS_PTHREAD=0" }