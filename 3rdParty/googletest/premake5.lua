project "GoogleTest"
   kind "StaticLib"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   defines { "GTEST_HAS_PTHREAD=0" }

   files { "include/gtest/**.h", "src/**.cc", "scr/**.h" }
   includedirs { "include", "." }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"