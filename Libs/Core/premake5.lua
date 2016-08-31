project "Core"
   kind "StaticLib"
   language "C++"
   targetdir "../../Bin/%{cfg.buildcfg}"
   includedirs { "Include" }
   links { "Test", "GoogleTest" }

   files { "Include/**.h", "Source/**.c", "Source/**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"