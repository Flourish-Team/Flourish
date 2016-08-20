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