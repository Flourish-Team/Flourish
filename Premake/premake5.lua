-- premake5.lua
workspace "Flourish"
   configurations { "Debug", "Release" }
   location("../Projects/" .. _ACTION)
   flags 
   { 
   		"Unicode", 
   		"ExtraWarnings", 
   		"FatalWarnings"
   }
   defines { "_HAS_EXCEPTIONS=0" }
   rtti ("Off")
   exceptionhandling ("Off")
   floatingpoint "Fast"
   startproject "TestProject" 

   	--Create x32/x64 platforms for each system (TODO: OSX/linux)
	filter { "system:windows" }
		platforms { "Win32", "Win64" }

	filter { "platforms:*64" }
	    architecture "x32"

	filter { "platforms:*64" }
	    architecture "x64"

   filter "configurations:Debug"
      defines { "_DEBUG", }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

	  -- Stripping symbols from OSX in release causes an error in LD
	  if os.get() == "macosx" then
	  flags { "Symbols" }
	  end

group("3rdParty")
	include "../3rdParty/googletest"
	
group("Flourish")
	include "../Libs/Core"
	include "../Libs/Test"

-- Include applications that need all the libs last
group("Examples")
	include "../TestProject"


