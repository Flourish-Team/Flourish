-- premake5.lua

require "cmake"

include "FlourishUtils"

workspace "Flourish"
   configurations { "Debug", "Release" }
   location("../Projects/" .. _ACTION)
   flags 
   { 
       "FatalWarnings",
   }
   characterset "Unicode"
   defines { "_HAS_EXCEPTIONS=0" }
   rtti ("Off")
   exceptionhandling ("Off")
   floatingpoint "Fast"
   startproject "UnitTestRunner" 
   warnings "Extra"
   cppdialect "C++17"

   --Create x32/x64 platforms for each system
	filter { "system:windows" }
		platforms { "Win32", "Win64" }

   filter { "system:macosx" }
      platforms { "OSX32", "OSX64" }
      -- Stripping symbols from OSX in release causes an error in LD
      symbols "On"

   filter { "system:linux" }
      platforms { "Linux32", "Linux64" }

	filter { "platforms:*32" }
	    architecture "x32"

	filter { "platforms:*64" }
	    architecture "x64"

   filter "configurations:Debug"
      defines { "_DEBUG", "FL_BUILD_CONFIG_DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG", "FL_BUILD_CONFIG_RELEASE" }
      optimize "On"

   filter { }

   --include libs
   group("3rdParty")
   	include "../3rdParty/googletest"
   
   --TODO: Generate this list automaticly by checking for premake files
   group("Flourish")
   	include "../Libs/Core"
      include "../Libs/UnitTests"

   -- Include applications that need all the libs last
   group("Tools")
   	include "../Tools/UnitTestRunner"

   --group("Examples") TODO
