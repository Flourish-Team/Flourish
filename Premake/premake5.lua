-- premake5.lua

require "cmake"

workspace "Flourish"
   configurations { "Debug", "Release" }
   location("../Projects/" .. _ACTION)
   filter { "action:cmake"}
      location("../")
   flags 
   { 
       "FatalWarnings",
   }
   characterset "Unicode"
   defines { "_HAS_EXCEPTIONS=0" }
   rtti ("Off")
   exceptionhandling ("Off")
   floatingpoint "Fast"
   startproject "TestProject" 
   warnings "Extra"
   cppdialect "C++11"

   	--Create x32/x64 platforms for each system (TODO: OSX/linux)
	filter { "system:windows" }
		platforms { "Win32", "Win64" }

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

	  -- Stripping symbols from OSX in release causes an error in LD
	  if os.target() == "macosx" then
	  symbols "On"
	  end

filter { }

group("3rdParty")
	include "../3rdParty/googletest"
	
group("Flourish")
	include "../Libs/Core"
	include "../Libs/Test"

-- Include applications that need all the libs last
group("Examples")
	include "../TestProject"


