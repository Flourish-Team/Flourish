project "GoogleTest"
   kind "StaticLib"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   files { "include/gtest/**.h", "src/**.cc", "scr/**.h" }
   includedirs { "include", "." }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"